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

int main(int argc, char *argv[ ])
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;
    struct sigaction sa;
    int yes = 1;

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
    my_addr.sin_port = htons(MYPORT);
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
        if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
            perror("Server-accept() error");
            continue;
        } else {
            printf("Server-accept() is OK...\n");
        }
        printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));

        /* this is the child process */
        if(!fork()) {
            close(sockfd);
            if(send(new_fd, "This is a test string from server!\n", 37, 0) == -1) {
                perror("Server-send() error lol!");
            }
            close(new_fd);
            exit(0);
        } else {
            close(new_fd);
            printf("Server-new socket, new_fd closed successfully...\n");
        }
    }
    return 0;
}
