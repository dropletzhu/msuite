/**
 * pim_sender.c: a pim packet generator, send pim packet one per
 *               second
 *  dropletzhu@gmail.com
 * 
 * ChangeLog
 *  - 2009-06-19
 * 		- add pim source option
 *		- add version information
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* General data type definition */
typedef unsigned char   UINT1;
typedef char            INT1;
typedef unsigned short  UINT2;
typedef unsigned long   UINT4;
typedef long            INT4;

/* Definitions for the message type */
#define  PIMSM_HELLO_MSG                             0
#define  PIMSM_REGISTER_MSG                          1
#define  PIMSM_REGISTER_STOP_MSG                     2
#define  PIMSM_JOIN_PRUNE_MSG                        3
#define  PIMSM_BOOTSTRAP_MSG                         4
#define  PIMSM_ASSERT_MSG                            5
#define  PIM_GRAFT_MSG                               6
#define  PIM_GRAFT_ACK_MSG                         	 7
#define  PIMSM_CRP_ADV_MSG                           8

/* Structure for Encoded Group Address */
/* ----------------------------------- */
typedef struct _SPimEncGrpAddr {
    UINT1  u1AddrFamily;
    UINT1  u1EncType;
    UINT1  u1Reserved;
    UINT1  u1MaskLen;
    UINT4  u4GrpAddr;
} tSPimEncGrpAddr;

/* Structure for Encoded Source Address */
/* ------------------------------------ */
typedef struct _SPimEncSrcAddr {
    UINT1  u1AddrFamily;
    UINT1  u1EncType;
    UINT1  u1Flags;
    UINT1  u1MaskLen;
    UINT4  u4SrcAddr;
} tSPimEncSrcAddr;

/* Structure for Encoded Unicast Address */
/* ------------------------------------- */
typedef struct _SPimEncUcastAddr {
    UINT1  u1AddrFamily;
    UINT1  u1EncType;
    UINT2  u2Alignbytes;  
    UINT4  u4UcastAddr;
} tSPimEncUcastAddr; 

/* Structure for Encoded Unicast Address */
/* ------------------------------------- */
struct _SPimEncUnAlignedUcastAddr {
    UINT1  u1AddrFamily;
    UINT1  u1EncType;
    UINT4  u4UcastAddr; 
} __attribute__ ((__packed__)) ;
 
typedef struct _SPimEncUnAlignedUcastAddr tSPimEncUnAlignedUcastAddr;

/* General PIM header */
#ifdef  _BIG_ENDIAN
typedef struct _SPimHdr {
    UINT1 pimver:4;
    UINT1 msgtpe:4;
    UINT1  u1Reserved;
    UINT2  u2Checksum;
} tSPimHdr;
#else
typedef struct _SPimHdr {
    UINT1 msgtpe:4;
    UINT1 pimver:4;
    UINT1  u1Reserved;
    UINT2  u2Checksum;
} tSPimHdr;
#endif

/* PIM register header */
#ifdef  _BIG_ENDIAN
typedef struct _SPimRegHdr {
    UINT1 pimver:4;
    UINT1 msgtpe:4;
    UINT1 u1Resv;
    UINT2 u2Checksum;
    UINT4 bdrbit:1;
    UINT4 nulbit:1;
    UINT4 rsvd:14;
} tSPimRegHdr;
#else 
typedef struct _SPimRegHdr {
    UINT1 msgtpe:4;
    UINT1 pimver:4;
    UINT1 u1Resv;
    UINT2 u2Checksum;
    UINT4 rsvd:14;
    UINT4 nulbit:1;
    UINT4 bdrbit:1;
} tSPimRegHdr;
#endif

/* Structure for Register stop Message */
/* ----------------------------------- */
typedef struct _SPimSmUnAlignedRegStopMsg {
    tSPimEncGrpAddr    EncGrpAddr;
    tSPimEncUnAlignedUcastAddr  EncUcastAddr;
}tSPimUnAlignedRegStopMsg;

csum (unsigned short *buf, int nwords)
{
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	return ~sum;
}

#define BUF_LEN 65535
#define IP_LEN 16
#define PORT_LEN 6

#define version "0.2"

char datagram[BUF_LEN];
char local_source[IP_LEN], rp[IP_LEN], mcast_source[IP_LEN], mcast_group[IP_LEN];
int mcast_port, count = 0, length = 256;

int make_pim_null_register()
{
	struct _SPimRegHdr *pim_reg_header;
	UINT2 *flags;

	pim_reg_header = (struct _SPimRegHdr*)(datagram); 
	pim_reg_header->msgtpe = PIMSM_REGISTER_MSG;
	pim_reg_header->pimver = 2;
	pim_reg_header->nulbit = 1;
	flags = (UINT2*)(datagram + 4);
	*flags = htons(*flags);
	pim_reg_header->u2Checksum = csum((unsigned short*)pim_reg_header,sizeof(struct _SPimRegHdr)>>1);

	return 0;
}

/* pim register header + ( multicast ip header + multicast udp header + udp payload ) */
int make_pim_register()
{
	struct _SPimRegHdr *pim_reg_header;
	struct ip *mcast_iph;
	struct udphdr *mcast_udphdr;

	pim_reg_header = (struct _SPimRegHdr*)(datagram); 
	mcast_iph = (struct ip*)(datagram + sizeof(struct _SPimRegHdr));
	mcast_udphdr = (struct udphdr*)(datagram + sizeof(struct _SPimRegHdr) + sizeof(struct ip));

	pim_reg_header->msgtpe = PIMSM_REGISTER_MSG;
	pim_reg_header->pimver = 2;
	pim_reg_header->u2Checksum = csum((unsigned short*)pim_reg_header,sizeof(struct _SPimRegHdr)>>1);

	mcast_iph->ip_hl = 5;
	mcast_iph->ip_v = 4;
	mcast_iph->ip_tos = 0;
	mcast_iph->ip_len = htons(length - sizeof(struct _SPimRegHdr));
	mcast_iph->ip_id = htonl(random());
	mcast_iph->ip_off = 0;
	mcast_iph->ip_ttl = 255;
	mcast_iph->ip_p = IPPROTO_UDP;
	mcast_iph->ip_sum = 0;
	mcast_iph->ip_src.s_addr = inet_addr (mcast_source);
	mcast_iph->ip_dst.s_addr = inet_addr (mcast_group);

	mcast_udphdr->source = htons(mcast_port);
	mcast_udphdr->dest = htons(mcast_port);
	mcast_udphdr->len = htons(length - sizeof(struct _SPimRegHdr) - sizeof(struct ip));
	mcast_udphdr->check = csum((unsigned short*)(((int*)mcast_udphdr)-2), strlen(((char*)mcast_udphdr)-1)>>1);

	mcast_iph->ip_sum = csum ((unsigned short*)mcast_iph, (length - sizeof(struct _SPimRegHdr))>>1);

	return 0;
}

int make_pim_register_stop()
{
	struct _SPimHdr *pim_header;

	pim_header = (struct _SPimHdr*)(datagram);
	pim_header->msgtpe = PIMSM_REGISTER_STOP_MSG;
	pim_header->pimver = 2;
	pim_header->u2Checksum = csum((unsigned short*)pim_header,(sizeof(struct _SPimHdr)+sizeof(tSPimEncGrpAddr) + sizeof(tSPimEncUnAlignedUcastAddr))>>1);

	return 0;
}

void usage()
{
	printf("Usage: ./pim_sender -i local_source -r rp -t type -s mcast_source -g mcast_group -p mcast_port -c count -l length\n");
	printf(" -i local_source	local source address\n");
	printf(" -r rp	rp address\n");
	printf(" -t type	1: null register;  2: register;  3: register-stop\n");	
	printf(" -s mcast_source	multicast payload source address\n");
	printf(" -g mcast_group		multicast payload group address\n");
	printf(" -p mcast_port		multicast payload port\n");
	printf(" -c count			the packet count, default count is unlimited\n");
	printf(" -l length			pim packet length, default is 256 bytes\n");
	printf(" Version: %s\n", version);
}

/* Send PIM unicast message */
int
main (int argc, char* argv[])
{
	char ch;
	int type;
	struct sockaddr_in local_addr, rp_addr;
	int i, retcode;
	int sockfd;

    if (argc <= 1) {
		usage();
        return 0;
    }

    while ((ch = getopt(argc, argv, "i:r:t:s:g:p:c:l:")) != -1)
    {
        switch (ch)
        {
			case 'i':
				strncpy(local_source, optarg, IP_LEN - 1);
				break;
			case 'r':
				strncpy(rp, optarg, IP_LEN - 1);
				break;
			case 't':
				type = atoi(optarg);
				if( (type < 1) || (type > 3) ) {
					printf("wrong type\n");
					return -1;
				}
			case 's':
				strncpy(mcast_source, optarg, IP_LEN -1);
				break;
			case 'g':
				strncpy(mcast_group, optarg, IP_LEN -1);
				break;
			case 'p':
				mcast_port = atoi(optarg);
				break;
			case 'c':
				count = atoi(optarg);
				break;
			case 'l':
				length = atoi(optarg);
				break;
        	default:
				usage();
            	return 0;
		}
    }

	sockfd = socket (PF_INET, SOCK_RAW, IPPROTO_PIM);
	if( sockfd < 0 ) {
		perror("socket\n");
		return -1;
	}

	memset(&local_addr, 0, sizeof(struct sockaddr_in));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = inet_addr(local_source);
	retcode = bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if( retcode != 0 ) {
		perror("bind\n");
		return -1;
	}

	memset(&rp_addr,0,sizeof(struct sockaddr_in));
	rp_addr.sin_family = AF_INET;
	rp_addr.sin_addr.s_addr = inet_addr (rp);

	i = 1;
	while (1)
    {
		if( count && ( i > count ) ) {
			break;
		}

		memset (datagram, 0, BUF_LEN);
		switch( type ) {
			case 1:
				make_pim_null_register();
				printf("Sender PIM null register %d\n",i++);
				break;
			case 2:
				make_pim_register();
				printf("Sender PIM register %d\n",i++);
				break;
			case 3:
				make_pim_register_stop();
				printf("Sender PIM register-stop %d\n",i++);
				break;
			default:
				printf("wrong type\n");
				return -1;
		}

   		if (sendto (sockfd, datagram, length, 0,(struct sockaddr *) &rp_addr,sizeof (rp_addr)) < 0) {
			perror("sendto\n");
			return -1;
		}
		sleep(1);
    }

	return 0;
}
