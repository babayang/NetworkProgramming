#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_COUNT_CLIENTS 100
int clients[MAX_COUNT_CLIENTS];
char is_active[MAX_COUNT_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void notify_all(char *buffer, int self) {
    for (int j = 0; j < MAX_COUNT_CLIENTS; j ++ ) {
        if (j == self) continue;
        if (is_active[j] == 0) continue;

        int n = write(clients[j], buffer, strlen(buffer)); // send on Windows

        if (n < -1) {
            perror("ERROR writing to socket");
            exit(0);
        }
    }
}

void client_handler(void *arg) {

    pthread_mutex_lock(&mutex);

    char *ind = (char *) arg;
    *ind = 1;
    int i = ind - is_active;
    int fd = clients[i];
    printf("%d has connected\n" , fd);

    pthread_mutex_unlock(&mutex);

    if (fd < -1) {
        perror("ERROR on accept");
        exit(0);
    }

    while (1) {
        char buffer[255];
        bzero(buffer, 255);
        int n = read(fd, buffer, 254); // recv on Windows

        if (n < -1) {
            perror("ERROR reading from socket");
            exit(0);
        }

        if (n == 0) {
            pthread_mutex_lock(&mutex);
            is_active[i] = 0;
            close(fd);
            pthread_mutex_unlock(&mutex);
            printf("%d exit \n", fd);
            break;

        }

        printf("%d send the message : %s\n",fd , buffer);
        fflush(stdout);

        // notify other clients
        notify_all(buffer, i);
    }



}
int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    uint16_t portno;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    ssize_t n;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);


    if (sockfd < -1) {
        perror("ERROR opening socket");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5000;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < -1) {
        perror("ERROR on binding");
        exit(0);
    }


    listen(sockfd, 4);
    clilen = sizeof(cli_addr);
    int cell = 0;

    // multithreading processes client requests
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        clients[cell] = newsockfd;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_handler, is_active + cell);
        pthread_detach(thread_id);
        cell ++ ;
        if (cell >= MAX_COUNT_CLIENTS) {
            perror("error");
        }
    }

    pthread_attr_destroy(&mutex);
    return 0;
}
