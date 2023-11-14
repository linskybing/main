#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define URL_SIZE 100
#define IP_SIZE 256
#define BUFFER_SIZE 100
#define PORT 80
#define HYPER_LIKE_SIZE 200
#define REQUEST_SIZE 300
char* conver_ip(char* url) {

    struct addrinfo hints;
    struct addrinfo *res, *tmp;
    static char host[256];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;

    int ret = getaddrinfo(url, NULL, &hints, &res);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
    }

    for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
        getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0,
                    NI_NUMERICHOST);
    }

    freeaddrinfo(res);
 
    return host;
}


int main() {    
    char* url = malloc(URL_SIZE * sizeof(char));
    char* domain_name = malloc(HYPER_LIKE_SIZE * sizeof(char));
    char* file_path = malloc(HYPER_LIKE_SIZE * sizeof(char));


    printf("Please enter the URL:\n");
    scanf("%s", url); 

    // seperate url into ip and file path
    strcpy(file_path, strstr(url, "/") + 1);
    domain_name = strtok(url, "/"); 

    char* ip_host = conver_ip(domain_name);

    printf("============ Socket ============ \n");

    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    // constitute message
    char *message = malloc(REQUEST_SIZE * sizeof(char));
    strcat(message, "GET /");
    if (file_path)
        strcat(message, file_path);
    strcat(message, " HTTP/1.1\r\nHost: ");
    strcat(message, domain_name);
    strcat(message, "\r\nConnection: close\r\n\r\n");

    //puts(message);

    unsigned char* buffer = malloc(BUFFER_SIZE * sizeof(char));
    unsigned char* hyper = malloc(HYPER_LIKE_SIZE * sizeof(char));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_host);
    server_addr.sin_port = htons(PORT);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    } 

    if (connect(sockfd, (struct sockaddr *)&server_addr, addrlen) == -1) {
        perror("connect()");
        fprintf(stderr, "Please start the server first\n");
        exit(EXIT_FAILURE);
    } 

    printf("Sending HTTP request\n");
    send(sockfd, message, strlen(message), 0);


    // Consider using recv() in a loop for large data to ensure complete message reception
    int len = 0;
    int count_hyper = 0;

    int flag = 0;
    int offset = 0;
    printf("Receiving the response\n");
    printf("============ Hyperlinks ============ \n");
    while((len = recv(sockfd, buffer + offset, BUFFER_SIZE - offset, 0)) > 0) {
        unsigned char *start, *target, *end;        
        unsigned char *cur = buffer;
        buffer[len + offset] = '\0';
        
        while ((start = strstr(cur, "<a")) &&
                (target = strstr(start, "href=\"")) &&
                (end = strstr(start, ">")) && (start - buffer) < len) {
                    
                target += 6;

                for (; *target != '\"'; target++) {
                    printf("%c", (*target));
                }

                count_hyper++;
                cur = end;
                printf("\n");
               
        }

        if ((start = strstr(cur, "<a"))) {
            flag = 1;
            for(offset = 0; *start != ">" && *start != '\0'; offset++, start++) {
                buffer[offset] = *start;
            }
            offset++;
        }
        else if (buffer[len-1] == '<') {
            flag = 1;
            buffer[0] = '<';
            offset++;
        }
        else {
            offset = 0;
        }
        //printf("%.*s", (len - 1), buffer);
    }

    printf("==================================== \n");
    printf("We have found %d hyperlinks\n", count_hyper);
    close(sockfd);
    return 0;

}