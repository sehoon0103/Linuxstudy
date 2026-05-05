#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_NAME "/tmp/echo_socket"

int main() {
    pid_t pid;
    int sockfd, connfd;
    struct sockaddr_un addr;
    int recv_bytes;
    char buf[256];

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "[Server] Failed to create socket\n");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path) - 1, SOCKET_NAME);

    unlink(addr.sun_path); // Remove the socket file if exists
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "[Server] Failed to bind\n");
        return 2;
    }

    if (listen(sockfd, 0) < 0) {
        fprintf(stderr, "[Server] Failed to listen\n");
        return 2;
    }

    while (1) {
        connfd = accept(sockfd, NULL, NULL);
        if (connfd < 0) {
            fprintf(stderr, "[Server] Failed to accept\n");
            break;
        }

        printf("[Server] Client connected!\n");

        pid = fork();
        if (pid == 0) {
            while (1) {
                recv_bytes = read(connfd, buf, sizeof(buf));
                if (recv_bytes <= 0) {
                    break;
                }
                printf("[Server] Recv : %.*s\n", recv_bytes, buf);
                write(connfd, buf, recv_bytes);
            }

            close(connfd);
            printf("[Server] Client disconnected!\n");
            return 0;
        }
    }

    close(sockfd);
    return 3;
}
