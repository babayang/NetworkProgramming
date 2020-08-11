#include "dns_protocol.h"

int main(int argc, char **argv) {
    if (argc != 1) {
        error_handing("argc error");
    }
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDR, &server_addr.sin_addr);
    socklen_t server_len = sizeof(server_addr);

    struct sockaddr reply_addr;
	socklen_t len = sizeof (reply_addr);

    char send_line[MAXLINE], recv_line[MAXLINE + 1];
    
    int n;

    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        int i = strlen(send_line);
        if (send_line[i - 1] == '\n') {
            send_line[i - 1] = 0;
        }

        // printf("now sending %s\n", send_line);
        size_t rt = sendto(socket_fd, send_line, strlen(send_line), 0, (struct sockaddr *) &server_addr, server_len);

        // printf("send bytes: %zu \n", rt);

        

        n = recvfrom(socket_fd, recv_line, MAXLINE, 0, &reply_addr, &len);
        recv_line[n] = 0;
        fputs(recv_line, stdout);
        fputs("\n", stdout);
    }

    exit(0);
}


