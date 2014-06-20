#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10 

void sigchld_handler(int s)
{
    while(wait(NULL) > 0);
}

void child_process(int socket_fd)
{
	int result;
	fd_set readset;
	char buffer[2048];

	while (1) {
		do {
			FD_ZERO(&readset);
			FD_SET(socket_fd, &readset);
			result = select(socket_fd + 1, &readset, NULL, NULL, NULL);
		} while (result == -1 && errno == EINTR);

		if (result > 0) {
			if (FD_ISSET(socket_fd, &readset)) {
				do {
					result = recv(socket_fd, buffer, 2048, 0);
				} while (result > 0);

				if (result < 0) {
					printf("Error on recv(): %s\n", strerror(errno));
				}
			}
		} else if (result < 0) {
			printf("Error on select(): %s\n", strerror(errno));
			break;
		}
	}
}

int main(int argc, char *argv[ ])
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;
    struct sigaction sa;
    int yes = 1;
	int port;

	if (argc != 2) {
		printf("Usage: ./tcpserver port\n");
		return -1;
	}
	port = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Server-socket() error lol!");
        exit(1);
    } else {
        printf("Server-socket() sockfd is OK...\n");
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Server-setsockopt() error lol!");
        exit(1);
    } else {
        printf("Server-setsockopt is OK...\n");
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);

    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("Server-bind() error");
        exit(1);
    } else {
        printf("Server-bind() is OK...\n");
    }

    if(listen(sockfd, BACKLOG) == -1) {
        perror("Server-listen() error");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Server-sigaction() error");
        exit(1);
    } else {
        printf("Server-sigaction() is OK...\n");
    }

    /* accept() loop */
    while(1) {
        sin_size = sizeof(struct sockaddr_in);
        if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,(socklen_t*)&sin_size)) == -1) {
            perror("Server-accept() error");
            continue;
        } else {
            printf("Server-accept() is OK...\n");
        }
        printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));

        /* this is the child process */
        if(!fork()) {
            close(sockfd);
			child_process(new_fd);
            close(new_fd);
            exit(0);
        } else {
            close(new_fd);
            printf("Server-new socket, new_fd closed successfully...\n");
        }
    }
    return 0;
}
