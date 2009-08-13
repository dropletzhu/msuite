/**
 * msender.c 
 *  send multicast packet one per second
 *
 * dropletzhu@gamil.com
 *
 * ChangeLog
 *  - 2009-08-12
 *		- add ip header manually, because on some linux platform,
 *        the ipip does not increment when sends packet
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
#include <netinet/ip.h>
#include <netinet/udp.h>

#define version "0.3"

#define IP_LEN 16
#define PORT_LEN 6
#define BUF_LEN 65535

/* udp/tcp pseudo header for checksum */
typedef struct pseudo_header_ {
	unsigned int src;
	unsigned int dst;
	unsigned char zero;
	unsigned char proto;
	unsigned short length;
}pseudo_header_t;

unsigned short
csum (unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (unsigned short)~sum;
}

unsigned short
udp_csum(unsigned short *pseudo_header, unsigned short *buf, int nwords)
{
    unsigned long sum = 0;
	int  i = 0;

	for ( ; i < sizeof(pseudo_header_t)>>1; i++ )
		sum += *pseudo_header++;

    for ( ;nwords > 0; nwords--)
        sum += *buf++;

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (unsigned short)~sum;
}

void usage()
{
    printf("Usage: ./msender -s source -g group -p port -t ttl -l length -c count\n");
	printf(" -s source	the source ip\n");
	printf(" -g group	the group ip\n");
	printf(" -p port	the port\n");
	printf(" -t ttl		the ttl, default ttl is 1\n");
	printf(" -l length	the packet length, default length is 256 bytes\n");
	printf(" -c count	the packet count, default count is unlimited\n");
    printf(" Version	%s\n",version);
}

int
main (int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd, ch, i = 1;
	char ttl = 1;
	char msgbuf[BUF_LEN], source[IP_LEN], group[IP_LEN], port[PORT_LEN];
	struct in_addr interface_addr;
	int length = 256;
	int count = 0;
	struct ip *iph = (struct ip*)msgbuf;
	struct udphdr *udp = (struct udphdr*)(msgbuf + sizeof(struct ip));
	char *payload = msgbuf + sizeof(struct ip) + sizeof(struct udphdr);
	int hdrincl = 1;
	int retcode;
	pseudo_header_t pseudo;

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
	if ((fd = socket (AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
	{
		perror ("socket");
		return -1;
	}

	/* we need to build ip header manually */
	retcode = setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl));
	if( retcode != 0 ) {
		perror("setsockopt\n");
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
		if ( count && ( i > count ) ) {
			break;
		}
		memset(msgbuf,0,BUF_LEN);

		/* ip header */
    	iph->ip_hl = 5;
    	iph->ip_v = 4;
    	iph->ip_tos = 0;
    	iph->ip_len = htons((short)length);
		if ( i == 0 )
			iph->ip_id = 0;
		else
    		iph->ip_id = htons((short)i);
    	iph->ip_off = 0;
    	iph->ip_ttl = ttl;
    	iph->ip_p = IPPROTO_UDP;
    	iph->ip_src.s_addr = inet_addr (source);
    	iph->ip_dst.s_addr = inet_addr (group);
    	iph->ip_sum = csum((unsigned short*)iph,
					sizeof(struct ip)>>1);

		/* udp header */
		udp->source = htons((short)atoi(port));
		udp->dest = htons((short)atoi(port));
		udp->len = htons((short)(length - sizeof(struct ip)));
		pseudo.src = iph->ip_src.s_addr;
		pseudo.dst = iph->ip_dst.s_addr;
		pseudo.zero = 0;
		pseudo.proto = iph->ip_p;
		pseudo.length = udp->len;

		/* payload */
		snprintf(payload,
				length-sizeof(struct ip)-sizeof(struct udphdr),
				"Sender %s->%s: %d", source, group, i++);
		udp->check = udp_csum((unsigned short*)&pseudo,
				(unsigned short*)udp, 
				(length - sizeof(struct ip))>>1);

		if (udp->check == 0)
			udp->check = 0xFFFF;

		if (sendto(fd, msgbuf, length, 0, (struct sockaddr *) &addr, 
					sizeof (addr)) < 0)
		{
			perror ("sendto");
			return -1;
		}
		else
		{
			printf ("%s\n",payload);
		}
		sleep (1);
	}
	return 0;
}
