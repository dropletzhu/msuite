/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

int main(int argc, char **argv) {
    int sockfd, sport, dport, n, retcode;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct sockaddr_in cliaddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 4) {
       fprintf(stderr,"usage: %s <hostname> <sport> <dport>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    sport = atoi(argv[2]);
    dport = atoi(argv[3]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    bzero((char *) &cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(sport);
    printf("source port %d\n",ntohs(cliaddr.sin_port));
    retcode = bind(sockfd,(struct sockaddr*)&cliaddr, sizeof(cliaddr));
    if (retcode != 0 ) {
        perror("ERROR bind");
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(dport);

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    snprintf(buf, BUFSIZE, "Udpclient\n");

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serveraddr, serverlen);
    if (n < 0) 
      perror("ERROR in sendto");
    
    /* print the server's reply */
    n = recvfrom(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serveraddr, &serverlen);
    if (n < 0) 
      perror("ERROR in recvfrom");
    printf("Echo from server: %s", buf);
    return 0;
}
