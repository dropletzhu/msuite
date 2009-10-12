/**
 * msender6.c 
 *  send multicast packet one per second
 *
 * dropletzhu@gamil.com
 *
 * ChangeLog
 *  - 2009-09-27
 *      - copy from msender.c and revise for ipv6
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/if.h>

#define version "0.1"

#define IP_LEN 40
#define PORT_LEN 6
#define BUF_LEN 65535
#define NAME_LEN 10

void usage()
{
    printf("Usage: ./msender -s source -g group -p port -h hops -l length -c count\n");
	printf(" -s souruce	the source ip\n");
	printf(" -i ifname 	the outgoing interface name\n");
	printf(" -g group	the group ip\n");
	printf(" -p port	the port\n");
	printf(" -h hops	the hops, default hops is 1\n");
	printf(" -l length	the packet length, default length is 256 bytes\n");
	printf(" -c count	the packet count, default count is unlimited\n");
    printf(" Version	%s\n",version);
}

int
main (int argc, char *argv[])
{
	struct sockaddr_in6 addr;
	int fd, ch, i = 1;
	int hops = 1;
	char msgbuf[BUF_LEN], source[IP_LEN], group[IP_LEN], port[PORT_LEN];
	char ifname[NAME_LEN];
	unsigned int ifindex = 0;
	int length = 256;
	int count = 0;

	if (argc <= 1)
	{
	    usage();
	    return 0;
	}

	while ((ch = getopt (argc, argv, "s:i:g:p:h:l:c:")) != -1)
	{
		switch (ch)
		{
		case 's':
			memset(source,0,IP_LEN);
			strncpy(source, optarg, IP_LEN-1);
			break;
		case 'i':
			memset(ifname,0,NAME_LEN);
			strncpy(ifname, optarg, NAME_LEN - 1);
			break;
		case 'g':
			memset (group, 0, IP_LEN);
			strncpy (group, optarg, IP_LEN - 1);
			break;
		case 'p':
			memset (port, 0, PORT_LEN);
			strncpy (port, optarg, PORT_LEN - 1);
			break;
		case 'h':
			hops = atoi (optarg);
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

	fd = socket (AF_INET6, SOCK_DGRAM, 0 );

	if (fd < 0)
	{
		perror ("socket");
		return -1;
	}

	ifindex = if_nametoindex(ifname);

	/* set up destination address */
	memset (&addr, 0, sizeof (addr));
	addr.sin6_family = AF_INET6;
	inet_pton(AF_INET6,group,&(addr.sin6_addr));
	addr.sin6_port = htons (atoi (port));

	if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex,
		 sizeof (ifindex)))
	{
		perror ("setsockopt if");
		return -1;
	}

	if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof (hops)))
	{
		perror ("setsockopt hops");
		return -1;
	}

	/* now just sendto() our destination! */
	while (1)
	{
		if ( count && ( i > count ) ) {
			break;
		}
		memset(msgbuf,0,BUF_LEN);

		snprintf(msgbuf,length,"Sender %s->%s <>%d", source, group, i++);

		if (sendto(fd, msgbuf, length, 0, (struct sockaddr *) &addr, 
					sizeof (addr)) < 0)
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
	return 0;
}
