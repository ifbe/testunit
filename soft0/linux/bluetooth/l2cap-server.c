#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

int main(int argc, char **argv)
{
	int s, client, bytes_read;
	char buf[1024] = { 0 };
	socklen_t opt = sizeof(rem_addr);
	struct sockaddr_l2 loc_addr = { 0 }, rem_addr = { 0 };

	// allocate socket
	s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

	// bind socket to port 0x1001 of the first available 
	// bluetooth adapter
	loc_addr.l2_family = AF_BLUETOOTH;
	loc_addr.l2_bdaddr = *BDADDR_ANY;
	loc_addr.l2_psm = htobs(0x1001);

	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	listen(s, 1);

	// accept one connection
	while(1)
	{
		client = accept(s, (struct sockaddr *)&rem_addr, &opt);
	
		ba2str( &rem_addr.l2_bdaddr, buf );
		fprintf(stderr, "accepted connection from %s\n", buf);

		memset(buf, 0, sizeof(buf));

		// read data from the client
		while(1)
		{
			bytes_read = read(client, buf, sizeof(buf));
			if( bytes_read > 0 )
			{
				printf("received [%s]\n", buf);
			}
		}
	}

	// close connection
	close(client);
	close(s);
}
