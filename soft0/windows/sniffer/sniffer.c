/* Authors: perry peng
 *
 * Date: September 11, 2011
 *
 * Project Name: 
 * Project Version:
 *
 * Module: main.c
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Mstcpip.h>
#include <Iphlpapi.h>

#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Iphlpapi")

#define MAX_PACKET_LENGTH 65535

#define ERR_MSG(m)                            \
  fprintf(stderr, "%s [%d] ", __FILE__, __LINE__ - 2); \
  if (!print_err_msg(GetLastError(), NULL))    \
    fprintf(stderr, m);

#define WSA_ERR_MSG(m)                        \
  fprintf(stderr, "%s [%d] ", __FILE__, __LINE__ - 2); \
  if (!print_err_msg(WSAGetLastError(), NULL)) \
    fprintf(stderr, m);

#define FATAL_ERROR(m)                        \
  {                                           \
    ERR_MSG(m);                               \
    goto Exception;                           \
  }

#define WSA_FATAL_ERROR(m)                    \
  {                                           \
    WSA_ERR_MSG(m);                           \
    goto Exception;                           \
  }

#define PRINT_IP(h, t)                        \
  fprintf(stdout, (h));                       \
  //if (!print_addr_info((PIP_ADDR_LIST)adp_info->First##t##Address)) \
    //fprintf(stdout, "No %s Addresses\n", #t);

#define HALLOC(x)       HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))
#define REALLOC(p, x)   HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (p), (x))
#define HFREE(p)        HeapFree(GetProcessHeap(), 0, (p))

#define AALLOC(x)       _aligned_malloc((x), MEMORY_ALLOCATION_ALIGNMENT)
#define AFREE(x)        _aligned_free((x))

typedef PIP_ADAPTER_UNICAST_ADDRESS PIP_ADDR_LIST;

typedef union _ADDR_IN
{
  struct
  {
    short family;
    u_short port;
    u_short Words[20];
  };
  struct sockaddr_in ipv4;
  struct sockaddr_in6 ipv6;
} ADDR_IN, *LPADDR_IN;

typedef struct _tagPacketNode
{
  SLIST_ENTRY entry;
  WSAOVERLAPPED overlap;
  WSABUF wsa;
  SOCKET socket;
  DWORD index;
  DWORD flags;
  DWORD length;
  struct _tagPacketNode *forward;
} PACKET, *LPPACKET;

// WARNING !!!  All list items must be aligned on a MEMORY_ALLOCATION_ALIGNMENT boundary;
// WARNING !!!  otherwise, this function will behave unpredictably.
PSLIST_HEADER
  packet_list = NULL;

LPPACKET
  packet_free_list = NULL;

CRITICAL_SECTION
  crit_sec;

volatile int
  bLoop;

DWORD
  packet_index = 0,
  packet_count = 0;

int print_addr_info(PIP_ADDR_LIST);
int print_err_msg(long, char *);

BOOL WINAPI ctrl_callback(DWORD);
DWORD WINAPI work_thread(LPVOID);
void begin_recv_packet(SOCKET);

int get_key_input(LPDWORD, DWORD, int);
int get_socket_addr(LPADDR_IN, short, unsigned short);

int
main()
{
  DWORD
    i,
    sck_ioctl,
    threads_num;

  HANDLE
    *threads = NULL,
    iocp = NULL;

  SOCKET
    socket_raw = INVALID_SOCKET;

  SYSTEM_INFO
    sys_info;

  LPPACKET
    packet;

  ADDR_IN
    addr;

  WSADATA
    wsa_init;

  if (!SetConsoleCtrlHandler(ctrl_callback, TRUE))
    FATAL_ERROR("SetConsoleCtrlHandler() failed to install console handler.\n");

  // 初始化线程打印同步对象。
  InitializeCriticalSection(&crit_sec);

  // 初始化SOCKET。
  if (WSAStartup(MAKEWORD(2, 2), &wsa_init) ||
    LOBYTE(wsa_init.wVersion) != 2 ||
    HIBYTE(wsa_init.wVersion) != 2)
    WSA_FATAL_ERROR("Failed to initialize socket.\n");

  // 显示当前主机的网络适配器列表，供用户选择。
  if (!get_socket_addr(&addr, AF_UNSPEC, 0))
    goto Exception;

  // 创建新SOCKET。
  socket_raw = WSASocket(addr.family, SOCK_RAW, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (socket_raw == INVALID_SOCKET)
    WSA_FATAL_ERROR("Failed to create socket.\n");

  // 绑定到本地某个网卡。
  if (bind(socket_raw, (struct sockaddr *)&addr, sizeof(ADDR_IN)) == SOCKET_ERROR)
    WSA_FATAL_ERROR("Failed to call bind.\n");

  // 设置SOCKET I/O CODE，打开RCVALL，开启网卡混杂模式将接收所有到达此网卡上的数据包。
  sck_ioctl = RCVALL_ON;
  if (WSAIoctl(socket_raw, SIO_RCVALL, &sck_ioctl, sizeof(sck_ioctl), NULL, 0, &i, NULL, NULL) == SOCKET_ERROR)
    WSA_FATAL_ERROR("Failed to call WSAIoctl.\n");

  // 创建新的完成端口并绑定SOCKET句柄到完成端口。
  iocp = CreateIoCompletionPort((HANDLE)socket_raw, NULL, socket_raw, 0);
  if (iocp == NULL)
    FATAL_ERROR("Failed to call CreateIoCompletionPort.\n");

  // 获得当前系统识别到的处理器数量。
  GetSystemInfo(&sys_info);
  threads_num = sys_info.dwNumberOfProcessors * 2;

  // 为链表头申请内存。
  packet_list = (PSLIST_HEADER)AALLOC(sizeof(SLIST_HEADER));
  if (packet_list == NULL)
    FATAL_ERROR("Failed to allocated memory.");

  // 初始化链表。
  InitializeSListHead(packet_list);

  // 申请线程组所需的内存。
  threads = (HANDLE*)HALLOC(threads_num * sizeof(HANDLE));
  if (!threads)
    FATAL_ERROR("Failed to allocate memory for thread array.\n");

  bLoop = 1;
  // 创建线程组。
  for (i = 0; i < threads_num; i++)
  {
    threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)work_thread, (LPVOID)iocp, 0, NULL);
    if (threads[i] == NULL)
    {
      ERR_MSG("Failed to create thread array.");
      for (; i < threads_num; i++)
        threads[i] = NULL;
      goto InitThreadsFailed;
    }
  }

  // 等待用户输入退出键关闭程序。
  while (get_key_input(&i, 0, TRUE))
  {
    switch (i)
    {
    case VK_RETURN:
      // 开始扑获网络包。
      packet_index = 0;
      begin_recv_packet(socket_raw);
      break;
    }
  }

InitThreadsFailed:

  bLoop = 0;
  WaitForMultipleObjects(threads_num, threads, TRUE, INFINITE);

Exception:
  if (threads)
  {
    for (i = 0; i < threads_num; i++)
      CloseHandle(threads[i]);

    // 释放掉线程数组内存。
    HFREE(threads);
    threads = NULL;
  }

  if (socket_raw != INVALID_SOCKET)
    closesocket(socket_raw);

  if (iocp != NULL)
    CloseHandle(iocp);

  if (packet_list)
  {
    // 清除掉链表所有节点。
    InterlockedFlushSList(packet_list);

    // 释放掉链表头占用的内存。
    AFREE(packet_list);
    packet_list = NULL;
  }

  while (packet_free_list != NULL)
  {
    packet = packet_free_list;
    if (packet->wsa.buf)
      HFREE(packet->wsa.buf);

    // 指向前一个链表节点。
    packet_free_list = packet->forward;
    // 释放掉链单元占用的内存。
    AFREE(packet);
  }

  // 删除线程打印同步对象。
  DeleteCriticalSection(&crit_sec);

  WSACleanup();

  // 取消CtrlHandler处理列程。
  SetConsoleCtrlHandler(ctrl_callback, FALSE);

  return 0;
}

void
begin_recv_packet(socket_raw)
  SOCKET socket_raw;
{
  LPPACKET
    packet;

  DWORD
    error;

  // 先从队列中查看是否有空闲的内存单元。
  packet = (LPPACKET)InterlockedPopEntrySList(packet_list);
  if (packet == NULL)
  {
    // 如果在队列中没有找到，将重新新建一个内存单元供WSARecv使用。
    packet = (LPPACKET)AALLOC(sizeof(PACKET));
    if (packet == NULL)
    {
      ERR_MSG("Memory allocation failed.\n");
      return;
    }

    ZeroMemory(packet, sizeof(PACKET));

    // 新的内存单元的数据缓冲区内存申请。
    packet->wsa.len = MAX_PACKET_LENGTH;
    packet->wsa.buf = HALLOC(MAX_PACKET_LENGTH);
    if (packet->wsa.buf == NULL)
    {
      ERR_MSG("Memory allocation failed.\n");
      AFREE(packet);
      return;
    }

    InterlockedIncrement(&packet_count);

    // 保护对链表的访问。
    EnterCriticalSection(&crit_sec);

    // 任何新建立的数据缓冲区将存入另外一个列表中，确保释放时不会遗漏。
    packet->forward = packet_free_list;
    packet_free_list = packet;

    LeaveCriticalSection(&crit_sec);
  }

  // 接收包计数。
  InterlockedIncrement(&packet_index);

  // 填充信息。
  packet->index = packet_index;   // 全局的包索引。
  packet->socket = socket_raw;    // 使用此包的SOCKET句柄。
  packet->flags = 0;

  // 开始异步接收数据包。
  if (WSARecv(socket_raw, &packet->wsa, 1, &packet->length, &packet->flags, &packet->overlap, NULL) == SOCKET_ERROR)
  {
    if ((error = WSAGetLastError()) != WSA_IO_PENDING)
    {
      WSASetLastError(error);
      WSA_ERR_MSG("WSARecv was failed.\n");
    }
  }
}

BOOL WINAPI
ctrl_callback(event)
  DWORD event;
{
  DWORD
    length;

  INPUT_RECORD
    record;

  switch (event)
  {
  case CTRL_C_EVENT:
  case CTRL_CLOSE_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    fprintf(stdout, "System is exiting...\n");

    ZeroMemory(&record, sizeof(INPUT_RECORD));
    record.EventType = KEY_EVENT;
    record.Event.KeyEvent.bKeyDown = 1;
    record.Event.KeyEvent.wRepeatCount = 1;
    record.Event.KeyEvent.wVirtualKeyCode = VK_ESCAPE;

    // 模拟用户按下ESC按键。
    WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &record, 1, &length);

    Sleep(10);

    // 指示所有工作线程停止工作。
    if (bLoop)
      bLoop = 0;

    break;

  default:
    // unknown type--better pass it on.
    return FALSE;
  }

  return TRUE;
}

DWORD WINAPI
work_thread(data)
  LPVOID data;
{
  HANDLE
    iocp = (HANDLE)data;

  DWORD
    i,
    error,
    transferred;

  ULONG_PTR
    key;

  LPWSAOVERLAPPED
    overlap;

  LPPACKET
    packet;

  CHAR
    display[128];

  while (bLoop)
  {
    overlap = NULL;
    // 从完成端口队列中取得一个已经完成的I/O操作。
    if (!GetQueuedCompletionStatus(iocp, &transferred, &key, &overlap, 500))  //INFINITE, in milliseconds
    {
      error = GetLastError();
      if (error == WAIT_TIMEOUT)
        continue;

      // 没有数据，是其它原因退出线程。
      if (overlap == NULL)
        break;
    }

    // 取得链表单元指针。
    packet = (LPPACKET)(((LPBYTE)overlap) - sizeof(SLIST_ENTRY));

    // 完成一个接收后在处理数据前就可以立即接收下一个包。
    begin_recv_packet(packet->socket);

    // 接到来自WSARev的数据，开始处理包信息。
    sprintf(display, "RX[%04X], I: %04d L: %04d, ", GetCurrentThreadId(), packet->index, packet->length);

    for (i = 0; i < 16; i++)
      sprintf(&display[strlen(display)], "%02X ", (BYTE)packet->wsa.buf[i]);
    sprintf(&display[strlen(display)], "\n");

    fprintf(stdout, display);

    // 清除相关数据。
    ZeroMemory(&packet->overlap, sizeof(WSAOVERLAPPED));

    // 数据包使用完后再重先入队，以便让其它线程使用。
    InterlockedPushEntrySList(packet_list, (PSLIST_ENTRY)packet);
  }

  fprintf(stdout, "Thread %04X was exited.\n", GetCurrentThreadId());

  return 0;
}

int
print_err_msg(errorId, format)
  long errorId;
  char *format;
{
  LPSTR
    text = NULL;

  DWORD
    length;

  // Gets the message string by an error code.
  length = FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, errorId, 0,
    (LPSTR)&text,
    0, NULL);

  if (length == 0)
  {
    fprintf(stderr, "FormatMessage was failed, error:0x%08x, format:0x%08x\n", GetLastError(), errorId);
    return 0;
  }

  fprintf(stderr, "%08X:", errorId);

  if (format != NULL)
    fprintf(stderr, format, text);
  else
    fprintf(stderr, text);

  // Free the buffer allocate by the FormatMessage function.
  LocalFree(text);

  return 1;
}

int
get_key_input(key, exitkey, keydown)
  LPDWORD key;
  DWORD exitkey;
  BOOL keydown;
{
  DWORD
    length;

  INPUT_RECORD
    record;

  *key = -1;
  // 读取一组控制台事件记录。
  if (!ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &record, 1, &length) ||
      !length)
  {
    ERR_MSG("Failed to get user key events.");
    Sleep(100);
    // 读取键盘输入失败，返回TRUE进入下一个读取动作。
    return 1;
  }

  // 判断是否为键盘事件。
  if (record.EventType != KEY_EVENT ||
     !(keydown ? record.Event.KeyEvent.bKeyDown: !record.Event.KeyEvent.bKeyDown))
    return 1;

  // 返回当前KEY值。
  *key = record.Event.KeyEvent.wVirtualKeyCode;

  // 如果是指定的退出按键或ESC则返回FALSE。
  return !(*key == exitkey || *key == VK_ESCAPE);
}

int
print_addr_info(addrinfo)
  PIP_ADDR_LIST addrinfo;
{
  CHAR
    /*
      IPv6版本IP地址需要更多的储存空间。
      For an IPv4 address, this buffer should be large enough to hold at least 16 characters.
      For an IPv6 address, this buffer should be large enough to hold at least 46 characters.
     */
    ip_addr[64];

  DWORD
    bytes = 0;

  while (addrinfo != NULL)
  {
    if (bytes > 0)
      fprintf(stdout, "\n                ");

    // 将IP地址信息转换为字符串。
    bytes = sizeof(ip_addr);
    if (WSAAddressToString(addrinfo->Address.lpSockaddr, addrinfo->Address.iSockaddrLength, NULL, ip_addr, &bytes) == ERROR_SUCCESS)
      fprintf(stdout, ip_addr);
    else
    {
      switch (WSAGetLastError())
      {
      case ERROR_SUCCESS:
        fprintf(stdout, ip_addr);
        break;
      case WSAEFAULT:
        fprintf(stdout, "Invalid buffer space.");
        break;
      case WSAEINVAL:
        fprintf(stdout, "Incorrect parameter.");
        break;
      case WSANOTINITIALISED:
        fprintf(stdout, "Winsock 2 DLL has not been initialized.");
        return FALSE;
      case WSAENOBUFS:
        fprintf(stdout, "No buffer space is available.");
        break;
      }
    }

    addrinfo = addrinfo->Next;
  }

  if (bytes == 0)
    return 0;

  fprintf(stdout, "\n");
  return 1;
}

int
get_socket_addr(addr, family, port)
  LPADDR_IN addr;
  short family;
  unsigned short port;
{
  PIP_ADAPTER_ADDRESSES
    adp_info_list = NULL,
    adp_info;

  CONSOLE_SCREEN_BUFFER_INFO
    con_info;

  PIP_ADAPTER_UNICAST_ADDRESS
    uni_addr;

  LPADDR_IN
    addr_in = NULL;

  int
    succeed = 9,
    vista = (GetVersion() & 0xff) >= 6;

  DWORD
    i,
    bytes,
    index,
    result;

  // 为链表分配一定的储存空间。
  bytes = sizeof(IP_ADAPTER_ADDRESSES);
  adp_info_list = (PIP_ADAPTER_ADDRESSES)HALLOC(bytes);
  if (adp_info_list == NULL)
    FATAL_ERROR("Failed to allocate heap memory.\n");

  i = GAA_FLAG_INCLUDE_PREFIX;
  if (vista)
    i |= GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_INCLUDE_WINS_INFO;

  // 获得储存有网卡信息的链表。
  while ((result = GetAdaptersAddresses(AF_UNSPEC, i, NULL, adp_info_list, &bytes)) != ERROR_SUCCESS)
  {
    if (result != ERROR_BUFFER_OVERFLOW)
      FATAL_ERROR("Failed to get address information.\n");

    // 如果是内存不足，将重新申请内存，直到足够为止。
    adp_info_list = (PIP_ADAPTER_ADDRESSES)REALLOC(adp_info_list, bytes);
    if (adp_info_list == NULL)
      FATAL_ERROR("Failed to allocate heap memory.\n");
  }

  // 获得控制台窗口尺寸，防止内容太长影响输出格式。
  if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &con_info))
    con_info.dwSize.X = 1024;
  con_info.dwSize.X -= 22;

  index = 0;

  // 打印本机所有网卡信息。
  adp_info = adp_info_list;
  while (adp_info)
  {
    // 只需要列出Ethernet网卡。
    if (adp_info->IfType != IF_TYPE_ETHERNET_CSMACD &&
        adp_info->IfType != IF_TYPE_IEEE80211)
    {
      adp_info = adp_info->Next;
      continue;
    }
    adp_info->IfIndex = ++index;
    fprintf(stdout, "%d   Name:       ", adp_info->IfIndex);
    if (wcslen(adp_info->Description) > con_info.dwSize.X)
    {
      for (i = 0; i < con_info.dwSize.X; i++)
        fprintf(stdout, "%wC", adp_info->Description[i]);
      fprintf(stdout, " ...\n");
    }
    else
      fprintf(stdout, "%wS\n", adp_info->Description);

    fprintf(stdout, "    MAC:        ");
    if (adp_info->PhysicalAddressLength > 0)
    {
      for (i = 0; i < adp_info->PhysicalAddressLength; i++)
      {
        if (i > 0)
          fprintf(stdout, "-");
        fprintf(stdout, "%02X", adp_info->PhysicalAddress[i]);
      }
      fprintf(stdout, "\n");
    }
    else
      fprintf(stdout, "00-00-00-00-00-00\n");

    PRINT_IP("    A-IP:       ", Anycast);
    PRINT_IP("    M-IP:       ", Multicast);
    PRINT_IP("    U-IP:       ", Unicast);
    PRINT_IP("    DNS:        ", DnsServer);

    //fprintf(stdout, "    DNS Suffix: %wS\n", adp_info->DnsSuffix);

    if (vista)
    {
      PRINT_IP("    GATEWAY:    ", Gateway);
      PRINT_IP("    WINS:       ", WinsServer);

      fprintf(stdout, "    SPEED:      ");
      if (adp_info->OperStatus == IfOperStatusUp ||
          adp_info->OperStatus == IfOperStatusTesting)
      {
        //fprintf(stdout, "TX=%I64dMBps, ", adp_info->TransmitLinkSpeed / 1024 / 1024);
        //fprintf(stdout, "RX=%I64dMBps\n", adp_info->ReceiveLinkSpeed / 1024 / 1024);
      }
      else
        fprintf(stdout, "TX=0MBps, RX=0MBps\n");
    }

    fprintf(stdout, "    STATUS:     ");
    switch (adp_info->OperStatus)
    {
    case IfOperStatusUp:              fprintf(stdout, "Up\n"); break;
    case IfOperStatusDown:            fprintf(stdout, "Down\n"); break;
    case IfOperStatusTesting:         fprintf(stdout, "Testing\n"); break;
    case IfOperStatusUnknown:         fprintf(stdout, "Unknown\n"); break;
    case IfOperStatusDormant:         fprintf(stdout, "Dormant\n"); break;
    case IfOperStatusNotPresent:      fprintf(stdout, "Not Present\n"); break;
    case IfOperStatusLowerLayerDown:  fprintf(stdout, "Lower Layer Down\n"); break;
    }

    fprintf(stdout, "\n");
    adp_info = adp_info->Next;
  }

  fprintf(stdout, "Please select a network adapter:");

  adp_info = NULL;
  // 等待用户输入一个数值表示选择某块网卡。
  while (get_key_input(&i, 0, TRUE))
    if (i >= 0x30 && i <= 0x39)
      break;

  adp_info = adp_info_list;
  while (adp_info != NULL)
  {
    // 判断是否为当前用户所选择的网络适配器编号。
    if ((adp_info->IfIndex + 0x30) != i)
    {
      adp_info = adp_info->Next;
      continue;
    }

    // 此网络适配器没有一个有效的IP地址。
    if (adp_info->FirstUnicastAddress == NULL)
      break;

    // 指定IP类型由参数family定义。
    index = family;

    // 如果family未定义就默认为是IPV4地址。
    if (index == AF_UNSPEC)
      index = AF_INET;

    while (TRUE)
    {
      uni_addr = adp_info->FirstUnicastAddress;
      while (uni_addr != NULL)
      {
        addr_in = (LPADDR_IN)uni_addr->Address.lpSockaddr;
        if (addr_in->family == index)
          break;
        uni_addr = uni_addr->Next;
      }

      // 找到一个指定的地址。
      if (addr_in->family == index)
        break;

      // 如果指定了IP类型但未匹配到正确的IP。
      if (family != AF_UNSPEC)
      {
        index = 0;
        break;
      }

      // 如果没有找到任何IPV4地址，再找IPV6地址。
      switch (index)
      {
      case AF_INET:
        // IPV4没有就找IPV6.
        index = AF_INET6;
        break;
      case AF_INET6:
        // IPV6没有就找...。
        break;
      }

      // 决定何时退出。如果连IPV6也没有找到。
      if (index == AF_INET6)
      {
        // 什么IP地址也没有找到。
        index = 0;
        break;
      }
    }

    // 未能找到IPV4或IPV6地址。
    if (index == 0)
      break;

    // 计算地址结构长度。
    bytes = uni_addr->Address.iSockaddrLength;
    if (bytes == 0)
      break;

    i = sizeof(ADDR_IN);
    // 打印当前选择的IP地址可能是IPV4或IPV6。
    if (WSAAddressToString((LPSOCKADDR)addr_in, bytes, NULL, (char*)addr, &i) == ERROR_SUCCESS)
      fprintf(stdout, "\nOK, the IP address %s was selected.\n", (char*)addr);
    else
      fprintf(stdout, "\nOK, the adapter %c was selected.\n", i);

    // 将选择的网卡的IP地址传给socket挷定。
    if (bytes > sizeof(ADDR_IN))
      bytes = sizeof(ADDR_IN);
    memcpy(addr, addr_in, bytes);

    addr->family = addr_in->family;
    addr->port = port;

    succeed = 1;

    break;
  }

  if (!succeed)
  {
    if (adp_info == NULL)
      fprintf(stdout, "\nNo adapter found which match your selection.\n");
    else
    {
      if (index == 0)
      {
        fprintf(stdout, "\nNo IP address found which match");
        switch (family)
        {
        case AF_INET:
          fprintf(stdout, " IPV4.\n");
          break;
        case AF_INET6:
          fprintf(stdout, " IPV6.\n");
          break;
        }
      }
      else
        fprintf(stdout, "\nThe adapter %c doesn't have a valid IP address.\n", i);
    }
  }

Exception:
  if (adp_info_list != NULL)
    HFREE(adp_info_list);

  return succeed;
}