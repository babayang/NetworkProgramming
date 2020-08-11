#include "dns_protocol.h"

// 域名到ip地址转换处理函数
void process(char **addr, char *message, int *cnt) {
    struct hostent *host = gethostbyname(message);
    if (!host)
        error_handing("gethost error");
    int i = 0;
    for (; host->h_addr_list[i]; ++ i) {
        addr[i] = inet_ntoa(*(struct in_addr*)host->h_addr_list[i]);
//        printf("Ip addr %d: %s \n", i + 1,addr[i]);
    }
    *cnt = i;
}
int main(int argc, char **argv) {

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    socklen_t client_len;
    char message[MAXLINE];


    struct sockaddr_in client_addr;
    client_len = sizeof(client_addr);
    for (;;) {
        int n = recvfrom(socket_fd, message, MAXLINE, 0, (struct sockaddr *) &client_addr, &client_len);
        message[n] = 0;
        printf("received %d bytes: %s\n", n, message);

        char *addr[5];
        int cnt = 0;
        process(addr, message, &cnt);
        char ret[100];
        strcpy(ret, "");
        for (int i = 0; i < 1; ++ i) {
            strcat(ret, addr[i]);
            strcat(ret, "\n");
        }


        char send_line[MAXLINE];
        sendto(socket_fd, ret, strlen(ret), 0, (struct sockaddr *) &client_addr, client_len);

    }

}


