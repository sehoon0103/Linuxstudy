#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <comento.h>

int main(int argc, char *argv[]) {
    char buffer[2][4096] = {0};
    size_t len[2] = {4096, 4096};

    if (argc == 3 && !strcmp(argv[1], "create")) {
        create_keyring(atoi(argv[2]));
    } else if (argc == 3 && !strcmp(argv[1], "destroy")) {
        destroy_keyring(atoi(argv[2]));
    } else if (argc == 3 && !strcmp(argv[1], "test")) {
        snprintf(buffer[0], 4096, "Hello world~!!!\n");
        encrypt(atoi(argv[2]), buffer[0], len[0], buffer[1], &len[1]);
        decrypt(atoi(argv[2]), buffer[1], len[1], buffer[0], &len[0]);
        printf("Encrypt and decrypt : %.4096s", buffer[0]);
    } else {
        printf("Usage %s <create|destory|test> <id>\n", argv[0]);
    }

    return 0;
}
