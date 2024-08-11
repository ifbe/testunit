/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2018-2021 WireGuard LLC. All Rights Reserved.
 */

#include <winsock2.h>
#include <Windows.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <mstcpip.h>
//#include <ip2string.h>
#include <winternl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "wintun.h"

static WINTUN_CREATE_ADAPTER_FUNC *WintunCreateAdapter;
static WINTUN_CLOSE_ADAPTER_FUNC *WintunCloseAdapter;
static WINTUN_OPEN_ADAPTER_FUNC *WintunOpenAdapter;
static WINTUN_GET_ADAPTER_LUID_FUNC *WintunGetAdapterLUID;
static WINTUN_GET_RUNNING_DRIVER_VERSION_FUNC *WintunGetRunningDriverVersion;
static WINTUN_DELETE_DRIVER_FUNC *WintunDeleteDriver;
static WINTUN_SET_LOGGER_FUNC *WintunSetLogger;
static WINTUN_START_SESSION_FUNC *WintunStartSession;
static WINTUN_END_SESSION_FUNC *WintunEndSession;
static WINTUN_GET_READ_WAIT_EVENT_FUNC *WintunGetReadWaitEvent;
static WINTUN_RECEIVE_PACKET_FUNC *WintunReceivePacket;
static WINTUN_RELEASE_RECEIVE_PACKET_FUNC *WintunReleaseReceivePacket;
static WINTUN_ALLOCATE_SEND_PACKET_FUNC *WintunAllocateSendPacket;
static WINTUN_SEND_PACKET_FUNC *WintunSendPacket;

static HMODULE
InitializeWintun(void)
{
	HMODULE Wintun =
		LoadLibraryExW(L"wintun.dll", NULL, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!Wintun)
		return NULL;
#define X(Name) ((*(FARPROC *)&Name = GetProcAddress(Wintun, #Name)) == NULL)
	if (X(WintunCreateAdapter) || X(WintunCloseAdapter) || X(WintunOpenAdapter) || X(WintunGetAdapterLUID) ||
		X(WintunGetRunningDriverVersion) || X(WintunDeleteDriver) || X(WintunSetLogger) || X(WintunStartSession) ||
		X(WintunEndSession) || X(WintunGetReadWaitEvent) || X(WintunReceivePacket) || X(WintunReleaseReceivePacket) ||
		X(WintunAllocateSendPacket) || X(WintunSendPacket))
#undef X
	{
		DWORD LastError = GetLastError();
		FreeLibrary(Wintun);
		SetLastError(LastError);
		return NULL;
	}
	return Wintun;
}

static void CALLBACK
ConsoleLogger(_In_ WINTUN_LOGGER_LEVEL Level, _In_ DWORD64 Timestamp, _In_z_ const WCHAR *LogLine)
{
	SYSTEMTIME SystemTime;
	FileTimeToSystemTime((FILETIME *)&Timestamp, &SystemTime);
	WCHAR LevelMarker;
	switch (Level)
	{
	case WINTUN_LOG_INFO:
		LevelMarker = L'+';
		break;
	case WINTUN_LOG_WARN:
		LevelMarker = L'-';
		break;
	case WINTUN_LOG_ERR:
		LevelMarker = L'!';
		break;
	default:
		return;
	}

	int j;
	char tmp[0x100];
	for(j=0;j<0x100;j++){
		tmp[j] = LogLine[j];
		if(0 == tmp[j])break;
	}

	fprintf(
		stderr,
		"%04u-%02u-%02u %02u:%02u:%02u.%04u [%c] %s\n",
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond,
		SystemTime.wMilliseconds,
		LevelMarker,
		tmp);
}

static DWORD64 Now(VOID)
{
	LARGE_INTEGER Timestamp;
	NtQuerySystemTime(&Timestamp);
	return Timestamp.QuadPart;
}

static HANDLE QuitEvent;
static volatile BOOL HaveQuit;

static BOOL WINAPI
CtrlHandler(_In_ DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		printf("Cleaning up and shutting down...\n");
		HaveQuit = TRUE;
		SetEvent(QuitEvent);
		return TRUE;
	}
	return FALSE;
}

static void
PrintPacket(_In_ const BYTE *Packet, _In_ DWORD PacketSize)
{
	if (PacketSize < 20)
	{
		printf("Received packet without room for an IP header\n");
		return;
	}
	BYTE IpVersion = Packet[0] >> 4, Proto;
	WCHAR Src[46], Dst[46];
	if (IpVersion == 4)
	{
		RtlIpv4AddressToStringW((struct in_addr *)&Packet[12], Src);
		RtlIpv4AddressToStringW((struct in_addr *)&Packet[16], Dst);
		Proto = Packet[9];
		Packet += 20, PacketSize -= 20;
	}
	else if (IpVersion == 6 && PacketSize < 40)
	{
		printf("Received packet without room for an IP header\n");
		return;
	}
	else if (IpVersion == 6)
	{
		RtlIpv6AddressToStringW((struct in6_addr *)&Packet[8], Src);
		RtlIpv6AddressToStringW((struct in6_addr *)&Packet[24], Dst);
		Proto = Packet[6];
		Packet += 40, PacketSize -= 40;
	}
	else
	{
		printf("Received packet that was not IP\n");
		return;
	}
	if (Proto == 1 && PacketSize >= 8 && Packet[0] == 0)
		printf("Received IPv%d ICMP echo reply from %s to %s\n", IpVersion, Src, Dst);
	else
		printf("Received IPv%d proto 0x%x packet from %s to %s\n", IpVersion, Proto, Src, Dst);
}

static USHORT
IPChecksum(_In_reads_bytes_(Len) BYTE *Buffer, _In_ DWORD Len)
{
	ULONG Sum = 0;
	for (; Len > 1; Len -= 2, Buffer += 2)
		Sum += *(USHORT *)Buffer;
	if (Len)
		Sum += *Buffer;
	Sum = (Sum >> 16) + (Sum & 0xffff);
	Sum += (Sum >> 16);
	return (USHORT)(~Sum);
}

static void
MakeICMP(_Out_writes_bytes_all_(28) BYTE Packet[28])
{
	memset(Packet, 0, 28);
	Packet[0] = 0x45;
	*(USHORT *)&Packet[2] = htons(28);
	Packet[8] = 255;
	Packet[9] = 1;
	*(ULONG *)&Packet[12] = htonl((10 << 24) | (6 << 16) | (7 << 8) | (8 << 0)); /* 10.6.7.8 */
	*(ULONG *)&Packet[16] = htonl((10 << 24) | (6 << 16) | (7 << 8) | (7 << 0)); /* 10.6.7.7 */
	*(USHORT *)&Packet[10] = IPChecksum(Packet, 20);
	Packet[20] = 8;
	*(USHORT *)&Packet[22] = IPChecksum(&Packet[20], 8);
	printf("Sending IPv4 ICMP echo request to 10.6.7.8 from 10.6.7.7\n");
}

static DWORD WINAPI
ReceivePackets(_Inout_ DWORD_PTR SessionPtr)
{
	WINTUN_SESSION_HANDLE Session = (WINTUN_SESSION_HANDLE)SessionPtr;
	HANDLE WaitHandles[] = { WintunGetReadWaitEvent(Session), QuitEvent };

	while (!HaveQuit)
	{
		DWORD PacketSize;
		BYTE *Packet = WintunReceivePacket(Session, &PacketSize);
		if (Packet)
		{
			PrintPacket(Packet, PacketSize);
			WintunReleaseReceivePacket(Session, Packet);
		}
		else
		{
			DWORD LastError = GetLastError();
			switch (LastError)
			{
			case ERROR_NO_MORE_ITEMS:
				if (WaitForMultipleObjects(_countof(WaitHandles), WaitHandles, FALSE, INFINITE) == WAIT_OBJECT_0)
					continue;
				return ERROR_SUCCESS;
			default:
				printf("Packet read failed\n", LastError);
				return LastError;
			}
		}
	}
	return ERROR_SUCCESS;
}

static DWORD WINAPI
SendPackets(_Inout_ DWORD_PTR SessionPtr)
{
	WINTUN_SESSION_HANDLE Session = (WINTUN_SESSION_HANDLE)SessionPtr;
	while (!HaveQuit)
	{
		BYTE *Packet = WintunAllocateSendPacket(Session, 28);
		if (Packet)
		{
			MakeICMP(Packet);
			WintunSendPacket(Session, Packet);
		}
		else if (GetLastError() != ERROR_BUFFER_OVERFLOW){
			printf("Packet write failed\n");
			return 0;
		}

		switch (WaitForSingleObject(QuitEvent, 1000 /* 1 second */))
		{
		case WAIT_ABANDONED:
		case WAIT_OBJECT_0:
			return ERROR_SUCCESS;
		}
	}
	return ERROR_SUCCESS;
}

int __cdecl main(void)
{
	HMODULE Wintun = InitializeWintun();
	if (!Wintun){
		printf("InitializeWintun: errno=%d\n", GetLastError());
		return 0;
	}
	WintunSetLogger(ConsoleLogger);
	printf("Wintun library loaded\n");

	DWORD LastError;
	HaveQuit = FALSE;
	QuitEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	if (!QuitEvent)
	{
		printf("CreateEventW !QuitEvent errno=%d\n", GetLastError());
		goto cleanupWintun;
	}
	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		printf("SetConsoleCtrlHandler errno=%d\n", GetLastError());
		goto cleanupQuit;
	}

	WCHAR name[8] = {'D','e','m','o'};
	WCHAR type[8] = {'E','x','a','m','p','l','e'};
	GUID ExampleGuid = { 0xdeadbabe, 0xcafe, 0xbeef, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };
	WINTUN_ADAPTER_HANDLE Adapter = WintunCreateAdapter(name, type, &ExampleGuid);
	if (!Adapter){
		LastError = GetLastError();
		fprintf(stderr, "WintunCreateAdapter: errno=%d\n", LastError);
		goto cleanupQuit;
	}

	DWORD Version = WintunGetRunningDriverVersion();
	printf("Wintun v%u.%u loaded\n", (Version >> 16) & 0xff, (Version >> 0) & 0xff);

	MIB_UNICASTIPADDRESS_ROW AddressRow;
	InitializeUnicastIpAddressEntry(&AddressRow);
	WintunGetAdapterLUID(Adapter, &AddressRow.InterfaceLuid);
	AddressRow.Address.Ipv4.sin_family = AF_INET;
	AddressRow.Address.Ipv4.sin_addr.S_un.S_addr = htonl((10 << 24) | (6 << 16) | (7 << 8) | (7 << 0)); // 10.6.7.7
	AddressRow.OnLinkPrefixLength = 24; // This is a /24 network
	AddressRow.DadState = IpDadStatePreferred;
	LastError = CreateUnicastIpAddressEntry(&AddressRow);
	if (LastError != ERROR_SUCCESS && LastError != ERROR_OBJECT_ALREADY_EXISTS)
	{
		printf("CreateUnicastIpAddressEntry: lasterror=%d\n", LastError);
		goto cleanupAdapter;
	}

	WINTUN_SESSION_HANDLE Session = WintunStartSession(Adapter, 0x400000);
	if (!Session)
	{
		printf("WintunStartSession: lasterror=%d\n", GetLastError());
		goto cleanupAdapter;
	}

	printf("Launching threads and mangling packets...\n");

	HANDLE Workers[] = { CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceivePackets, (LPVOID)Session, 0, NULL),
						 CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendPackets, (LPVOID)Session, 0, NULL) };
	if (!Workers[0] || !Workers[1])
	{
		printf("CreateThread lasterror=%d\n", GetLastError());
		goto cleanupWorkers;
	}
	WaitForMultipleObjectsEx(_countof(Workers), Workers, TRUE, INFINITE, TRUE);
	LastError = ERROR_SUCCESS;

cleanupWorkers:
	HaveQuit = TRUE;
	SetEvent(QuitEvent);
	for (size_t i = 0; i < _countof(Workers); ++i)
	{
		if (Workers[i])
		{
			WaitForSingleObject(Workers[i], INFINITE);
			CloseHandle(Workers[i]);
		}
	}
	WintunEndSession(Session);
cleanupAdapter:
	WintunCloseAdapter(Adapter);
cleanupQuit:
	SetConsoleCtrlHandler(CtrlHandler, FALSE);
	CloseHandle(QuitEvent);
cleanupWintun:
	FreeLibrary(Wintun);
	return LastError;
}
