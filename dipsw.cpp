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
#include "thread_manager.h"

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
    pthread_mutex_lock(thread_manager::get_a());
    read(fd, &retvalue, 8);
    pthread_mutex_unlock(thread_manager::get_a());
    retvalue &= 0xFF;
    close(fd);

    return retvalue;
}
