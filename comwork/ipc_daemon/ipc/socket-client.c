#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_NAME "/tmp/echo_socket"

int main() {
    pid_t pid;
    int sockfd, recv_bytes;
    struct sockaddr_un addr;
    char buf[256];

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "[Client] Failed to create socket\n");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path) - 1, SOCKET_NAME);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "[Client] Failed to connect\n");
        return 2;
    }

    while (1) {
        printf("q for exit> ");

        buf[0] = '\0';

        scanf("%255s", buf);
        if (!strcmp(buf, "q")) {
            break;
        }

        write(sockfd, buf, strlen(buf));
        recv_bytes = read(sockfd, buf, sizeof(buf));
        if (recv_bytes <= 0) {
            printf("[Client] Server shutdown!\n");
            break;
        }

        printf("[Client] Recv : %.*s\n", recv_bytes, buf);
    }

    close(sockfd);

    printf("[Client] Exit!\n");

    return 3;
}
