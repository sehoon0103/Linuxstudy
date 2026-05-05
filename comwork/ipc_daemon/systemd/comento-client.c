#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>

#define SOCKET_NAME "/run/comento.sock"

#define MSG_TYPE_ENCRYPT 0
#define MSG_TYPE_DECRYPT 1

int request(char type, const char *in, size_t in_len, char *out, size_t *out_len)
{
    size_t pos, tmp_out_len;
    int sockfd;
    struct sockaddr_un addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return -errno;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path) - 1, SOCKET_NAME);

    if (connect(sockfd, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
        return -errno;
    }

    write(sockfd, &type, sizeof(type));
    write(sockfd, &in_len, sizeof(in_len));
    for (pos = 0; pos < in_len;) {
        pos += write(sockfd, in + pos, in_len - pos);
    }

    read(sockfd, &tmp_out_len, sizeof(tmp_out_len));
    if (*out_len > tmp_out_len) {
        *out_len = tmp_out_len;
    }

    for (pos = 0; pos < *out_len;) {
        pos += read(sockfd, out + pos, *out_len - pos);
    }

    close(sockfd);

    return 0;
}

int encrypt(const char *in, size_t in_len, char *out, size_t *out_len)
{
    return request(MSG_TYPE_ENCRYPT, in, in_len, out, out_len);
}

int decrypt(const char *in, size_t in_len, char *out, size_t *out_len)
{
    return request(MSG_TYPE_DECRYPT, in, in_len, out, out_len);
}

char msg_buf[4096];

int main() {
    pid_t pid;
    char msg_type;
    int sockfd;
    size_t msg_len, out_len;

    printf("Input > ");

    msg_buf[0] = '\0';
    scanf("%2048s", msg_buf);
    msg_len = strlen(msg_buf);

    out_len = 4096;
    printf("Req ENCRYPT %d %s\n", (int)msg_len, msg_buf);
    encrypt(msg_buf, msg_len, msg_buf, &out_len);
    printf("Rsp %d\n", (int)out_len);

    msg_len = out_len;
    out_len = 4096;
    printf("Req DECRYPT %d bytes data\n", (int)msg_len);
    decrypt(msg_buf, msg_len, msg_buf, &out_len);
    printf("Rsp %d %.*s\n", (int)out_len, (int)out_len, msg_buf);

    return 0;
}
