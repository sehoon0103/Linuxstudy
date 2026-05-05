#include <sys/ioctl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define COMENTO_CTL_DEVICE_NAME "keyringctl"
#define COMENTO_IOCTL_MAGIC 'C'
#define COMENTO_IOCTL_ADD _IOW(COMENTO_IOCTL_MAGIC, 0, int)
#define COMENTO_IOCTL_DEL _IOW(COMENTO_IOCTL_MAGIC, 1, int)

int main(int argc, char *argv[]) {
    unsigned long request;
    int fd, ret;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <add|del> <number>\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "add")) {
        request = COMENTO_IOCTL_ADD;
    } else if (!strcmp(argv[1], "del")) {
        request = COMENTO_IOCTL_DEL;
    } else {
        fprintf(stderr, "Usage: %s <add|del> <number>\n", argv[0]);
        return -1;
    }

    fd = open("/dev/" COMENTO_CTL_DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        printf("Failed to open device\n");
        return -1;
    }

    ret = ioctl(fd, request, atoi(argv[2]));
    if (ret < 0) {
        printf("Failed to do ioctl command : %d\n", ret);
        return -1;
    }

    return 0;
}
