#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define SHM_NAME "/comento_mem"
#define SHM_SIZE 4096

int main(int argc, char *const argv[]) {
    int needs_init = 0, fd;
    char *ptr;

    if (argc == 2 && !strcmp(argv[1], "-d")) {
        printf("Delete the shared memory\n");
        shm_unlink(SHM_NAME);
        return 0;
    } else if (argc == 2 && !strcmp(argv[1], "-l")) {
        static char *const ls_argv[] = {
            "/bin/ls", "/dev/shm", NULL
        };
        printf("List the shared memory :\n");
        execve(ls_argv[0], ls_argv, NULL);
        fprintf(stderr, "Failed to run ls command\n");
        return 5;
    } else if (argc != 1) {
        fprintf(stderr, "Usage : %s <-d> <-l>\n", argv[0]);
        return 4;
    }

    fd = shm_open(SHM_NAME, O_RDWR, 0600);
    if (fd == -1) {
        if (errno == ENOENT) {
            printf("Create new shared memory\n");
            fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
            printf("Set the size of shared memory\n");
            if (ftruncate(fd, SHM_SIZE) == -1) {
                fprintf(stderr, "Failed to truncate\n");
                return 2;
            }
            needs_init = 1;
        } else {
            fprintf(stderr, "Failed to shm_open\n");
            return 1;
        }
    }

    printf("Mapping the shared memory\n");
    ptr = (char *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap\n");
        return 3;
    }

    if (!needs_init) {
        printf("before : %.4095s\n", ptr);
    }

    scanf("%4095s", ptr);

    munmap(ptr, SHM_SIZE);
    close(fd);

    return 0;
}
