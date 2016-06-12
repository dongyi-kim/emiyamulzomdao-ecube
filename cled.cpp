//
// Created by root on 16. 6. 12.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "thread_manager.h"

using namespace std;

#define DRIVER_CLED		"/dev/cncled"

/*
void doHelp(void)
{
	printf("Usage:Color LED\n");
	printf("cledtest ledindex  red green blue\n");
	printf("color range 0 ~ 255\n");
	printf("ex) cledtest 0  255 255 255 \n");
	printf(" =>0th led is white color \n");
	printf("ex) cledtest 0 255 0 0 \n");
	printf(" =>0th led is Red color \n");
	printf("ledindex : 0 ~ 3 \n");
}
*/

#define INDEX_LED		0
#define INDEX_REG_LED		1
#define INDEX_GREEN_LED		2
#define INDEX_BLUE_LED		3
#define INDEX_MAX			4

void cledContr(int driverfile, int idx_led, int r, int g, int b)
{
    unsigned short colorArray[INDEX_MAX];
    colorArray[INDEX_LED] = idx_led;
    colorArray[INDEX_REG_LED] =(unsigned short) r;
    colorArray[INDEX_GREEN_LED] =(unsigned short) g;
    colorArray[INDEX_BLUE_LED] =(unsigned short) b;

    pthread_mutex_lock(thread_manager::get_a());
    write(driverfile, &colorArray ,6);
    pthread_mutex_unlock(thread_manager::get_a());

    return;
}


void _cled(Shared* shared)
{
    int fd;

    // open  driver
    fd = open(DRIVER_CLED, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver  open error.\n");
        return;
    }

    bool liq_prev = !shared->liq_exist;

    while(1) {
        if( liq_prev != shared->liq_exist ) {
            if (shared->liq_exist) {
                cledContr(fd, 0, 0, 0, 0);
            }
            else {
                cledContr(fd, 0, 0, 0, 255);
            }

        }

        liq_prev = shared->liq_exist;
    /*
        if () {
            cledContr(fd, 1, 0, 255, 0);
        }
        else {
            cledContr(fd, 1, 0, 0, 0);
        }

        if () {
            cledContr(fd, 2, 255, 0, 0);
        }
        else {
            cledContr(fd, 2, 0, 0, 0);
        }
        if () {
            cledContr(fd, 3, 0, 0, 0);
        }
        else {
            cledContr(fd, 3, 0, 0, 0);
        }
        */
        usleep(100000);

    }
    close(fd);

    return;
}

void* cled(void* shared) {
    _cled((Shared*)shared);
}