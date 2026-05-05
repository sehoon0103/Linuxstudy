#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SEM_NAME "comento"

int main(int argc, char *argv[]) {
    int *ptr, i, sem_init = 1;
    sem_t *sem;
    pid_t pid;

    if (argc == 2 && !strcmp(argv[1], "--no-sem")) {
        sem_init = 2;
    }

    sem_unlink(SEM_NAME); // Remove semaphore if exists
    sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, sem_init);
    if (sem == SEM_FAILED) {
        fprintf(stderr, "Failed to create semaphore\n");
        return 1;
    }

    printf("Mapping an anonymous shared memory\n");
    ptr = mmap(NULL, sizeof(*ptr), PROT_READ | PROT_WRITE,
               MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid = fork();
    if (pid == 0) {
        for (i = 0; i < 10000; i++) {
            sem_wait(sem);
            (*ptr)++; // load *ptr, reg; inc reg, 1; store reg, *ptr
            sem_post(sem);
            if (i % 1000 == 0) {
                printf("[Child ] %d\n", *ptr);
            }
        }
    } else {
        for (i = 0; i < 10000; i++) {
            sem_wait(sem);
            (*ptr)--; // load *ptr, reg; dec reg, 1; store reg, *ptr
            sem_post(sem);
            if (i % 1000 == 0) {
                printf("[Parent] %d\n", *ptr);
            }
        }
    }

    wait(NULL);

    printf("Final value : %d\n", *ptr);

    return 0;
}
