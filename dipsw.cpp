#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DRIVER_DIPSW		"/dev/cndipsw"

int dipsw()
{
    int fd;
    int retvalue;

    // open  driver
    fd = open(DRIVER_DIPSW, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver  open error.\n");
        return -1;
    }
    read(fd, &retvalue, 8);
    retvalue &= 0xFF;
    printf("0x%X\n", retvalue);
    close(fd);

    return retvalue;
}
