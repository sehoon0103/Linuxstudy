#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <systemd/sd-daemon.h>

#define KEY "This is a password!!!"
#define TMP_KEY_FILE "/tmp/comento.key"
#define TMP_DECRYPT_FILE "/tmp/comento.decrypt"
#define TMP_ENCRYPT_FILE "/tmp/comento.encrypt"

#define MSG_TYPE_ENCRYPT 0
#define MSG_TYPE_DECRYPT 1

char * cmd_argv[] = {
    "/usr/bin/openssl",
    "enc",
    NULL, // [2], it will be -d or -e
    "-aes-256-cbc",
    "-kfile",
    TMP_KEY_FILE,
    "-in",
    NULL, // [7], will be TMP_DECRYPT_FILE or TMP_ENCRYPT_FILE
    "-out",
    NULL, // [9], will be TMP_DECRYPT_FILE or TMP_ENCRYPT_FILE
    NULL
};

int main(int argc, char *argv[]) {
    int num_fds, recv_bytes, fd, conn_fd, file_fd;
    char msg_type = -1, *msg_buf;
    unsigned long long msg_len = 0, pos;
    pid_t pid;

    num_fds = sd_listen_fds(0);
    daemon(0, 1);

    if (num_fds < 0) {
        fprintf(stderr, "Failed to get listen_fds\n");
        fflush(stdout);
        return 1;
    }

    file_fd = open(TMP_KEY_FILE, O_CREAT | O_TRUNC | O_WRONLY, 0400);
    write(file_fd, KEY, strlen(KEY));
    close(file_fd);

    for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + num_fds; fd++) {

        conn_fd = accept(fd, NULL, NULL);
        if (conn_fd <= 0) {
            fprintf(stderr, "Failed to accept");
            goto err;
        }

        read(conn_fd, &msg_type, sizeof(msg_type));
        read(conn_fd, &msg_len, sizeof(msg_len));

        msg_buf = malloc(msg_len);
        for (pos = 0; pos < msg_len;) {
            pos += read(conn_fd, msg_buf + pos, msg_len - pos);
        }

        printf("Req type %d, len %d\n", msg_type, msg_len);
        fflush(stdout);

        switch (msg_type) {
            case MSG_TYPE_ENCRYPT:
                cmd_argv[2] = "-e";
                cmd_argv[7] = TMP_DECRYPT_FILE;
                cmd_argv[9] = TMP_ENCRYPT_FILE;
                break;
            case MSG_TYPE_DECRYPT:
                cmd_argv[2] = "-d";
                cmd_argv[7] = TMP_ENCRYPT_FILE;
                cmd_argv[9] = TMP_DECRYPT_FILE;
                break;
            default:
                fprintf(stderr, "No proper msg type");
                goto err;
        }

        file_fd = open(cmd_argv[7], O_CREAT | O_TRUNC | O_WRONLY, 0600);
        write(file_fd, msg_buf, msg_len);
        close(file_fd);
        free(msg_buf);

        pid = fork();
        if (pid != 0) {
            wait(NULL);
        } else {
            execve(cmd_argv[0], cmd_argv, NULL);
        }

        file_fd = open(cmd_argv[9], O_CREAT | O_RDONLY, 0600);
        msg_len = lseek(file_fd, 0, SEEK_END);
        lseek(file_fd, 0, SEEK_SET);
        msg_buf = malloc(msg_len);
        read(file_fd, msg_buf, msg_len);
        close(file_fd);

        write(conn_fd, &msg_len, sizeof(msg_len));
        for (pos = 0; pos < msg_len;) {
            pos += write(conn_fd, msg_buf + pos, msg_len - pos);
        }
        free(msg_buf);

        printf("Rsp len %d\n", msg_len);
        fflush(stdout);

err:
        close(conn_fd);
        close(fd);
    }

    return 0;
}
