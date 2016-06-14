/*
    @file   -   dipsw.cpp
    @author -   
    @brief  -   read that dip switch is on or off.
    @reference - /root/periApp/dipswtest/dipswtest.c(system programming practice 7th device driver.pptx)
*/

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

#define DRIVER_DIPSW		"/dev/cndipsw"///< dipswitch file driver path

int dipsw()
{
    int fd;
    int retvalue;

    fd = open(DRIVER_DIPSW, O_RDWR);///< open dip switch file driver
    if ( fd < 0 )
    {
        perror("driver  open error.\n");
        return -1;
    }
    pthread_mutex_lock(thread_manager::get_dips());///< waiting for printing picture of oled
    read(fd, &retvalue, 8);
    pthread_mutex_unlock(thread_manager::get_dips());
    retvalue &= 0xFF;///< and operation what is dip switch turn on.
    close(fd);///< close file driver.

    return retvalue;
}
