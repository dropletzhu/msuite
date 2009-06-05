/*
 * A simple PIM packet test tool
 * by dropletzhu 2009/4/14
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

#define BUF_LEN 256
#define IP_LEN 16
#define PORT_LEN 6

/* Receive PIM packet */
int
main (int argc, char* argv[])
{
	char ch;
	char datagram[BUF_LEN];
	char rp[IP_LEN];
	struct sockaddr_in sin;
	struct _SPimHdr *pim_header;
	struct _SPimRegHdr *pim_reg_header;
	int sockfd;
	int pim_null_reg = 0, pim_reg = 0, pim_reg_stop = 0;
	int len,nbytes;
	UINT2 *flags;

    if (argc <= 1) {
        printf("Usage: ./pim_linster -r rp\n");
        return 0;
    }

    while ((ch = getopt(argc, argv, "r:")) != -1)
    {
        switch (ch)
        {
			case 'r':
				strncpy(rp, optarg, IP_LEN -1);
				break;

        	default:
        		printf("Usage: ./pim_linster -r rp\n");
            return 0;
		}
    }

	sockfd = socket (PF_INET, SOCK_RAW, IPPROTO_PIM);
	if( sockfd < 0 ) {
			perror("socket\n");
			return -1;
	}

	memset(&sin,0,sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr (rp);

	if (bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
			perror("bind\n");
			return -1;
	}

	len = sizeof(sin);
	while (1)
    {
		memset (datagram, 0, BUF_LEN);

    	if ( (nbytes = recvfrom(sockfd, datagram,BUF_LEN, 0,(struct sockaddr *) &sin, &len)) < 0) {
			perror("recvfrom\n");
			return -1;
		} else {
			pim_header = (struct _SPimHdr*)(datagram + sizeof(struct ip)); 
			if( pim_header->msgtpe == PIMSM_REGISTER_MSG ) {
				pim_reg_header = (struct _SPimRegHdr*)(datagram + sizeof(struct ip));
				flags = (UINT2*)(datagram + sizeof(struct ip) + 4 );
				*flags = ntohs(*flags);
				if( pim_reg_header->nulbit == 1 ) {
					printf("Receiver PIM null register %d\n",pim_null_reg++);
				} else {
					printf("Receiver PIM register %d\n",pim_reg++);
				}
			} else if( pim_header->msgtpe == PIMSM_REGISTER_STOP_MSG ) {
				printf("Receiver PIM register-stop %d\n",pim_reg_stop++);
			} else {
				printf(" unknown type: %d, nbytes: %d\n", pim_header->msgtpe, nbytes);
			}
		}
    }

	return 0;
}
