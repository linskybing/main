#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#define BUFFER_SIZE 1024
#define PORT 8080

int main(int argc, char const *argv[]) {
    struct addrinfo hints;
    struct addrinfo *res, *tmp;
    char host[256];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;

    if (argc != 2) {
        fprintf(stderr, "%s string\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    puts(argv[1]);
    int ret = getaddrinfo(argv[1], NULL, &hints, &res);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
    }

    for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
        getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0,
                    NI_NUMERICHOST);
        puts(host);
    }

    freeaddrinfo(res);
    exit(EXIT_SUCCESS);
}
