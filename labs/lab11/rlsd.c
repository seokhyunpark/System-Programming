#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <time.h>

#define PORTNUM 15000
#define HOSTLEN 256
#define oops(msg) {perror(msg); exit(1);}

void sanitize(char *str);
int main(int argc, char *argv[]) {
    struct sockaddr_in saddr;
    struct hostent *hp;
    char hostname[HOSTLEN];
    int sock_id, sock_fd;
    FILE *sock_fpi, *sock_fpo;
    FILE *pipe_fp;
    char dirname[BUFSIZ];
    char command[BUFSIZ];
    int dirlen, c;

    // Step 1: ask kernel for a socket
    sock_id = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_id == -1)
        oops("socket");

    // Step 2: bind address (host, port) to socket
    bzero((void *)&saddr, sizeof(saddr));
    gethostname(hostname, HOSTLEN);
    hp = gethostbyname(hostname);
    // src -> dest
    bcopy((void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
    saddr.sin_port = htons(PORTNUM);
    saddr.sin_family = AF_INET;
    // now bind
    if (bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
        oops("bind");
    
    // Step 3: allow incoming calls with Qsize=1 on socket
    if (listen(sock_id, 1) != 0)
        oops("listen");
    while (1) {
        sock_fd = accept(sock_id, NULL, NULL);
        if (sock_fd == -1)
            oops("accept");
        // open reading direction as buffered stream
        if ((sock_fpi = fdopen(sock_fd, "r")) == NULL)
            oops("fdopen reading");
        if (fgets(dirname, BUFSIZ - 5, sock_fpi) == NULL)
            oops("reading dirname");
        // sanitize dirname
        sanitize(dirname);
        // open writing direction as buffered stream
        if ((sock_fpo = fdopen(sock_fd, "w")) == NULL)
            oops("fdopen writing");
        // get command
        sprintf(command, "ls %s", dirname);
        if ((pipe_fp = popen(command, "r")) == NULL)
            oops("popen");
        // transfer data from ls to socket
        while ((c = getc(pipe_fp)) != EOF)
            putc(c, sock_fpo);
        pclose(pipe_fp);
        fclose(sock_fpo);
        fclose(sock_fpi);
    }

    return 0;
}

// check string to see if anything risky
void sanitize(char *str) {
    char *src, *dest;
    for (src = dest = str; *src; src++) {
        // check a char for alphabet or digit
        if (*src == '/' || isalnum(*src))
            *dest++ = *src;
    }
    *dest = '\0'; // null-terminated string
}
