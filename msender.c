/*
 * sender.c: send multicast packet one per second
 *  dropletzhu@gamil.com
 *
 * ChangeLog
 *  - 2009-06-04
 *		- Add '-c' option for sending number of packets
 *		- Show source ip address on command line
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define version "2009-06-04"

#define IP_LEN 16
#define PORT_LEN 6
#define TTL_LEN 4
#define BUF_LEN 65535

void usage()
{
    printf("Usage: ./msender -s source -g group -p port -t ttl -l length  -c count\n");
    printf(" Version: %s\n",version);
    printf(" Default length is 256 bytes\n");
    printf(" Default ttl is 1\n");
}

int
main (int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd, cnt, ch, i = 0;
	char ttl = 1;
	struct ip_mreq mreq;
	char msgbuf[BUF_LEN], source[IP_LEN], group[IP_LEN], port[PORT_LEN];
	struct in_addr interface_addr;
	int length = 256;
	int count = 0;

	if (argc <= 1)
	{
	    usage();
	    return 0;
	}

	while ((ch = getopt (argc, argv, "s:g:p:t:l:c:")) != -1)
	{
		switch (ch)
		{
		case 's':
			memset(source,0,IP_LEN);
			strncpy(source, optarg, IP_LEN - 1);
			break;
		case 'g':
			memset (group, 0, IP_LEN);
			strncpy (group, optarg, IP_LEN - 1);
			break;
		case 'p':
			memset (port, 0, PORT_LEN);
			strncpy (port, optarg, PORT_LEN - 1);
			break;
		case 't':
			ttl = atoi (optarg);
			break;
		case 'l':
			length = atoi(optarg);
			break;
		case 'c':
			count = atoi(optarg);
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

	/* set up source address */
	interface_addr.s_addr = inet_addr (source);

	/* set up destination address */
	memset (&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr (group);
	addr.sin_port = htons (atoi (port));

	if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &interface_addr,
		 sizeof (interface_addr)))
	{
		perror ("setsockopt");
		return -1;
	}

	if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof (ttl)))
	{
		perror ("setsockopt");
		return -1;
	}

	/* now just sendto() our destination! */
	while (1)
	{
		if ( count && ( i >= count ) ) {
			break;
		}

		memset(msgbuf,0,BUF_LEN);
		snprintf(msgbuf,length,"Sender %s->%s: %d", source, group, i++);

		if (sendto(fd, msgbuf, length, 0, (struct sockaddr *) &addr, sizeof (addr)) < 0)
		{
			perror ("sendto");
			return -1;
		}
		else
		{
			printf ("%s\n",msgbuf);
		}
		sleep (1);
	}
}
