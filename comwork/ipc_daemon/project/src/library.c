#define SOCKET_NAME "/run/comento.sock"
#define MSG_TYPE_ENCRYPT 0
#define MSG_TYPE_DECRYPT 1
#define MSG_TYPE_CREATE 2
#define MSG_TYPE_DESTROY 3

#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


static int request(char type, uid_t uid, const char *in, size_t in_len,
                   char *out, size_t *out_len)
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
    write(sockfd, &uid, sizeof(uid));
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

int encrypt(uid_t uid, const char *in, size_t in_len,
            char *out, size_t *out_len)
{
    return request(MSG_TYPE_ENCRYPT, uid, in, in_len, out, out_len);
}

int decrypt(uid_t uid, const char *in, size_t in_len,
            char *out, size_t *out_len)
{
    return request(MSG_TYPE_DECRYPT, uid, in, in_len, out, out_len);
}

int create_keyring(uid_t uid)
{
    int ret, out;
    size_t out_len = sizeof(out);
    ret = request(MSG_TYPE_CREATE, uid, NULL, 0, (char*)&out, &out_len);
    if (ret == 0) ret = out;
    return ret;
}

int destroy_keyring(uid_t uid)
{
    int ret, out;
    size_t out_len = sizeof(out);
    ret = request(MSG_TYPE_DESTROY, uid, NULL, 0, (char*)&out, &out_len);
    if (ret == 0) ret = out;
    return ret;
}
