/*
* mlistener.c:  joins a multicast group and echoes all data it receives 
* 				from the group to its stdout
* dropletzhu@gmail.com
*
* ChangeLog
* 	- 2009-06-04
* 		- Show source and group address on console
*/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define version "0.2"

#define IP_LEN 16
#define PORT_LEN 6
#define BUF_LEN 65535

void usage()
{
	printf("Usage: ./mlistener -s source -g group -p port -l length \n");
	printf(" -s souurce	use the source to select bind device\n");
	printf(" -g group	the group to listen\n");
	printf(" -p port	the port to listen\n");
	printf(" -l length	the packet length, default length is 256 bytes,\n");
	printf("		    it should be consistent with msender\n");
	printf(" Version 	%s\n", version);
}

int
main (int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd, nbytes, addrlen, ch, i = 0;
	struct ip_mreq mreq;
	char msgbuf[BUF_LEN], source[IP_LEN], group[IP_LEN], port[PORT_LEN];
	int length = 256;

	if (argc <= 1)
	{
		usage();
		return 0;
	}

	while ((ch = getopt (argc, argv, "s:g:p:l:")) != -1)
	{
		switch (ch)
		{
		case 's':
			memset (source, 0, IP_LEN);
			strncpy (source, optarg, IP_LEN - 1);
			break;
		case 'g':
			memset (group, 0, IP_LEN);
			strncpy (group, optarg, IP_LEN - 1);
			break;
		case 'p':
			memset (port, 0, PORT_LEN);
			strncpy (port, optarg, PORT_LEN - 1);
			break;
		case 'l':
			length = atoi(optarg);
			break;
		default:
			usage();
			return 0;
		}
	}

	/* create what looks like an ordinary UDP socket */
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror ("socket");
		return -1;
	}

	/* set up destination address */
	memset (&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr (group);
	addr.sin_port = htons (atoi (port));

	/* bind to receive address */
	if (bind (fd, (struct sockaddr *) &addr, sizeof (addr)) < 0)
	{
		perror ("bind");
		return -1;
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr = inet_addr (group);
	mreq.imr_interface.s_addr = inet_addr (source);

	if (setsockopt (fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof (mreq)) < 0)
	{
		perror ("setsockopt");
		return -1;
	}

	while (1)
	{
		memset(msgbuf, 0, BUF_LEN);

		addrlen = sizeof (addr);
		if ((nbytes = recvfrom (fd, msgbuf, length, 0, (struct sockaddr *) &addr, &addrlen)) < 0)
		{
			perror ("recvfrom");
			return -1;
		}
		printf ("Receiver %d -- %s\n", i++, msgbuf);
	}
}
