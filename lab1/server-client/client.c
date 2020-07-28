#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

int flag = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// send data
char force_send(int sockfd, char *buffer, int len) {
    int n = len;

    while (n) {
        int r = write(sockfd, buffer + len - n, n);
        n -= r;
    }
}


char send_message(int sockfd,  char *text) {
//    printf("%s", text);
    force_send(sockfd, text, strlen(text));
}

// the create thread is responsible for reading server feedback and pringting to standard output
void server_handler(void *arg) {
    int *sockfd = (int *) arg;

    while (1) {
        char buf[255];

        int n = read(*sockfd, buf, 254);

        if (n == 0) {
            close(sockfd);
            exit(1);
        }

        buf[n] = 0;
        pthread_mutex_lock(&mutex);

        if (flag) {
            printf("%s", buf);
        }

        pthread_mutex_unlock(&mutex);
    }

}
int main(int argc, char *argv[]) {
    int sockfd, n;
    uint16_t portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = (uint16_t) atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    // the create thread is responsible for reading server feedback and pringting to standard output
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, server_handler, &sockfd);
    pthread_detach(&thread_id);
//    pthread_join(thread_id, NULL);


    // processing standard input and sending it to server
    while (1) {
        char buffer[256];
        bzero(buffer, 256);
        time_t t = time(NULL);
        struct tm* lt = localtime(&t);

        char hours[10], minute[10];
        sprintf(hours, "%d", lt->tm_hour);
        sprintf(minute, "%d", lt->tm_min);

        char fmt[1024];
        strcpy(fmt, "{");
        strcat(fmt, hours);
        strcat(fmt, ":");
        strcat(fmt, minute);
        strcat(fmt, "} [");
        strcat(fmt, argv[3]);
        strcat(fmt, "] ");

//        printf("%s", fmt);

        if (fgets(buffer, 255, stdin) != NULL) {
            if (*buffer == 'm' || *buffer == 'M') {
                pthread_mutex_lock(&mutex);
                flag = 0;
                pthread_mutex_unlock(&mutex);
                continue;
            }

            strcat(fmt, buffer);
//            printf("%s", fmt);

            pthread_mutex_lock(&mutex);

            send_message(sockfd,  fmt);
            flag = 1;

            pthread_mutex_unlock(&mutex);
        }
    }
    close(sockfd);

    return 0;

}
