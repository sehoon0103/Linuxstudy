#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define COMENTO_DEVICE_NAME "comento-device"
#define COMENTO_MAGIC 'C'
#define COMENTO_DEVICE_IOCTL_CLEAR _IO(COMENTO_MAGIC, 0)

int main(int argc, char *argv[])
{
    int fd = open("/dev/" COMENTO_DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        printf("Failed to open device\n");
        return -1;
    }

    if (ioctl(fd, COMENTO_DEVICE_IOCTL_CLEAR, 0) < 0) {
        printf("Failed to do ioctl command\n");
        return -1;
    }

    close(fd);
    return 0;
}
