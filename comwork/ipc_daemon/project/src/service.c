#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <systemd/sd-daemon.h>

#define TMP_DECRYPT_FILE "/tmp/comento.decrypt"
#define TMP_ENCRYPT_FILE "/tmp/comento.encrypt"
#define COMENTO_CTL_DEVICE_NAME "keyringctl"
#define COMENTO_IOCTL_MAGIC 'C'
#define COMENTO_IOCTL_ADD _IOW(COMENTO_IOCTL_MAGIC, 0, int)
#define COMENTO_IOCTL_DEL _IOW(COMENTO_IOCTL_MAGIC, 1, int)

#define MSG_TYPE_ENCRYPT 0
#define MSG_TYPE_DECRYPT 1
#define MSG_TYPE_CREATE 2
#define MSG_TYPE_DESTROY 3

static void crypto(int encrypt, uid_t uid, char **msg_buf, size_t *msg_len) {
    int file_fd;
    pid_t pid;
    char dev_path[64], *cmd_argv[] = {
        "/usr/bin/openssl",
        "enc",
        NULL, // [2] will be -d or -e
        "-aes-256-cbc",
        "-kfile",
        dev_path,
        "-in",
        NULL, // [7] will be TMP_DECRYPT_FILE or TMP_ENCRYPT_FILE
        "-out",
        NULL, // [9] will be TMP_DECRYPT_FILE or TMP_ENCRYPT_FILE
        NULL
    };

    snprintf(dev_path, 64, "/dev/keyring%d", uid);
    if (encrypt) {
        cmd_argv[2] = "-e";
        cmd_argv[7] = TMP_DECRYPT_FILE;
        cmd_argv[9] = TMP_ENCRYPT_FILE;
    } else {
        cmd_argv[2] = "-d";
        cmd_argv[7] = TMP_ENCRYPT_FILE;
        cmd_argv[9] = TMP_DECRYPT_FILE;
    }

    file_fd = open(cmd_argv[7], O_CREAT | O_TRUNC | O_WRONLY, 0600);
    write(file_fd, *msg_buf, *msg_len);
    close(file_fd);
    free(*msg_buf);

    pid = fork();
    if (pid != 0) {
        wait(NULL);
    } else {
        execve(cmd_argv[0], cmd_argv, NULL);
    }

    file_fd = open(cmd_argv[9], O_CREAT | O_RDONLY, 0600);
    *msg_len = lseek(file_fd, 0, SEEK_END);
    lseek(file_fd, 0, SEEK_SET);
    *msg_buf = malloc(*msg_len);
    read(file_fd, *msg_buf, *msg_len);
    close(file_fd);
}

static void manage_keyring(int create, uid_t uid, char **msg_buf, size_t *msg_len) {
    int fd, *ret;
    size_t request;
    *msg_len = sizeof(int);
    *msg_buf = malloc(*msg_len);
    ret = (int*)*msg_buf;
    if (create) {
        request = COMENTO_IOCTL_ADD;
    } else {
        request = COMENTO_IOCTL_DEL;
    }
    fd = open("/dev/" COMENTO_CTL_DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        printf("Failed to open device\n");
        *ret = -1;
        return;
    }
    *ret = ioctl(fd, request, uid);
    printf("manage_keyring!! %d %x %d %d\n", fd, request, uid, *ret);
}

int main(int argc, char *argv[]) {
    int num_fds, recv_bytes, fd, conn_fd;
    char msg_type = -1, *msg_buf;
    size_t msg_len = 0, pos;
    uid_t uid;

    num_fds = sd_listen_fds(0);
    daemon(0, 1);
    if (num_fds < 0) {
        fprintf(stderr, "Failed to get listen_fds\n");
        fflush(stdout);
        return 1;
    }

    for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + num_fds; fd++) {
        conn_fd = accept(fd, NULL, NULL);
        if (conn_fd <= 0) {
            fprintf(stderr, "Failed to accept");
            goto exit;
        }
        read(conn_fd, &msg_type, sizeof(msg_type));
        read(conn_fd, &uid, sizeof(uid));
        read(conn_fd, &msg_len, sizeof(msg_len));
        if (msg_len != 0) {
            msg_buf = malloc(msg_len);
            for (pos = 0; pos < msg_len;) {
                pos += read(conn_fd, msg_buf + pos, msg_len - pos);
            }
        }
        printf("Req type %d, uid %d, len %d\n", msg_type, uid, msg_len);
        fflush(stdout);
        switch (msg_type) {
            case MSG_TYPE_ENCRYPT:
                crypto(1, uid, &msg_buf, &msg_len);
                break;
            case MSG_TYPE_DECRYPT:
                crypto(0, uid, &msg_buf, &msg_len);
                break;
            case MSG_TYPE_CREATE:
                manage_keyring(1, uid, &msg_buf, &msg_len);
                break;
            case MSG_TYPE_DESTROY:
                manage_keyring(0, uid, &msg_buf, &msg_len);
                break;
            default:
                fprintf(stderr, "No proper msg type");
                goto exit;
        }

        write(conn_fd, &msg_len, sizeof(msg_len));
        for (pos = 0; pos < msg_len;) {
            pos += write(conn_fd, msg_buf + pos, msg_len - pos);
        };
        free(msg_buf);

        printf("Rsp len %d\n", msg_len);
        fflush(stdout);

exit:
        close(conn_fd);
        close(fd);
    }

    return 0;
}
