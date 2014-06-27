#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void short_lived_connection(int sockfd)
{
	char buffer[512];
	int retcode;

	memset(buffer, 0, 512);
	retcode = send(sockfd, buffer, 512, 0);
	printf("send %d bytes\n", retcode);

	retcode = recv(sockfd, buffer, 512, 0);
	printf("recv %d bytes\n", retcode);
}

void long_lived_connection(int sockfd)
{
	char buffer[1024];
	int retcode;

	memset(buffer, 0, 1024);
	while (1) {
		retcode = send(sockfd, buffer, 1024, 0);
		if (retcode < 0) {
			perror("send()");
			break;
		}
	}
}

void usage()
{
	printf("Usage: ./tcpclient -s [server_ip] -p [port] -l\n");
	printf(" -s	server ip address\n");
	printf(" -p  server port\n");
	printf(" -l  long-lived connection, by default, it is short-lived echo connection\n");
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in their_addr;
	struct in_addr server_ip;
	int port;
	char ch;
	int retcode;
	int long_lived = 0;

    if (argc < 2) {
        usage();
        return -1;
    }

    while ((ch = getopt (argc, argv, "s:p:l")) != -1) {
        switch (ch) {
		case 's':
			retcode = inet_aton(optarg, &server_ip);
			if (retcode == 0) {
				printf("wrong address format\n");
				return -1;
			}
        case 'p':
            port = atoi(optarg);
            break;
        case 'l':
            long_lived = 1;
            break;
        default:
            usage();
            return -1;
        }
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(-1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    their_addr.sin_addr.s_addr = server_ip.s_addr;
    memset(&(their_addr.sin_zero), '\0', 8);

    if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect()");
        exit(-1);
    }

	if (long_lived) {
		long_lived_connection(sockfd);
	} else {
		short_lived_connection(sockfd);
	}
 
    close(sockfd);
    return 0;
}
