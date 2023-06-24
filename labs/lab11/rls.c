#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define opps(msg) {perror(msg); exit(1);}
#define PORTNUM 15000

int main(int argc, char *argv[]) {
    struct sockaddr_in servadd;
    struct hostent *hp;
    int sock_id, sock_fd;
    char buffer[BUFSIZ];
    int n_read;

    if (argc != 3)
        exit(-1);
    // Step 1: Get a socket
    sock_id = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_id == -1)
        opps("socket");
    // Step 2: connect to server
    bzero(&servadd, sizeof(servadd));
    hp = gethostbyname(argv[1]);
    if (hp == NULL)
        opps(argv[1]);
    bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr, hp->h_length);
    // Refer to https://linux.die.net/man/3/htons
    // convert values from host to network byte order
    servadd.sin_port = htons(PORTNUM);
    servadd.sin_family = AF_INET;
    if (connect(sock_id, (struct sockaddr *)&servadd, sizeof(servadd)) != 0)
        opps("connect");
    // Step 3: send directory name, then read back results
    if (write(sock_id, argv[2], strlen(argv[2])) == -1)
        opps("write");
    if (write(sock_id, "\n", 1) == -1)
        opps("write");
    while ((n_read = read(sock_id, buffer, BUFSIZ)) > 0)
        if (write(1, buffer, n_read) == -1)
            opps("write");
    close(sock_id);
}
