#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define __NR_push_data 451
#define __NR_pop_data 452

int main(int argc, char *argv[]) {
    long data, result;

    if (!strcmp(argv[1], "push") && argc == 3) {
        data = atol(argv[2]);
        result = syscall(__NR_push_data, data);
        if (result != 0) {
            fprintf(stderr, "Failed to push!\n");
        }
    } else if (!strcmp(argv[1], "pop") && argc == 2) {
        result = syscall(__NR_pop_data);
        if (result != -1) {
            printf("Value : %ld\n", result);
        } else {
            fprintf(stderr, "Failed to pop!\n");
        }
    } else {
        fprintf(stderr, "Usage : %s <push | pop> [data]\n", argv[0]);
        return -1;
    }

    return 0;
}
