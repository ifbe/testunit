#include <stdio.h>
#include <winsock2.h>
#include <windows.h> 

int main (int argc, char * argv []) 
{ 
	// Check command line parameters 
	if(argc != 2) 
	{
	 printf ( "Usage:%s <MAC address>\n", argv [0]); 
	 printf ( "Example:%s 00-D0-4C-BF-52-BA\n", argv [0]); 
	 return 0; 
	}

	int dstaddr [6]; 
	int i = sscanf(argv [1],
		"%2x-%2x-%2x-%2x-%2x-%2x", 
		& dstaddr [0],
		& dstaddr [1],
		& dstaddr [2],
		& dstaddr [3],
		& dstaddr [4],
		& dstaddr [5]
	); 
	if(i != 6) 
	{ 
	 fprintf (stderr, "Invalid MAC Adresse!"); 
	 return 1; 
	}

	unsigned char ether_addr[6]; 
	for(i=0; i<6; i++)ether_addr[i] = dstaddr[i];

	// Magic Packet structure
	u_char magicpacket [200];
	memset(magicpacket, 0xff, 6);
	int packetsize = 6;
	for(i=0; i<16; i++)
	{
	 memcpy (magicpacket + packetsize, ether_addr, 6); 
	 packetsize += 6; 
	}

	// Start the WSA 
	WSADATA WSAData; 
	if (WSAStartup (MAKEWORD (2, 0), &WSAData) != 0)
	{ 
	 fprintf (stderr, "WSAStartup failed:%d\n", GetLastError()); 
	 return 1; 
	}

	// Create socket 
	SOCKET sock = socket (AF_INET, SOCK_DGRAM, 0); 
	if (sock == INVALID_SOCKET) 
	{ 
	 fprintf (stderr, "Socket create error:%d\n", GetLastError ()); 
	 return 1; 
	}

	// Set to broadcast sent 
	BOOL bOptVal = TRUE; 
	int iOptLen = sizeof (BOOL); 
	if (setsockopt (sock, SOL_SOCKET, SO_BROADCAST, (char *) & bOptVal, iOptLen) == SOCKET_ERROR) 
	{ 
	 fprintf (stderr, "setsockopt error:%d\n", WSAGetLastError ()); 
	 closesocket (sock); 
	 WSACleanup (); 
	 return 1; 
	}

	struct sockaddr_in to;
	to.sin_family = AF_INET; 
	to.sin_port = htons (0); 
	to.sin_addr.s_addr = htonl (INADDR_BROADCAST);

	// Send Magic Packet 
	if (sendto (sock, (const char *) magicpacket, packetsize, 0, (const struct sockaddr *) & to, sizeof (to)) == SOCKET_ERROR) 
	printf ("Magic packet send error:% d", WSAGetLastError ()); 
	else printf ( "Magic packet send!");

	closesocket (sock); 
	WSACleanup (); 
	return 0; 
}
