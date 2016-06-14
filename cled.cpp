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
#include "dipsw.h"

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

    pthread_mutex_lock(thread_manager::get_cled());
    write(driverfile, &colorArray ,6);
    pthread_mutex_unlock(thread_manager::get_cled());

    return;
}


void _cled(Shared* shared)
{
    int fd;
    int dip;
    int preDip = -1;
    // open  driver
    fd = open(DRIVER_CLED, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver  open error.\n");
        return;
    }

    bool liq_prev = !shared->liq_exist;

    while(1) {
        dip = dipsw();//load dip switch value
        if(dip & 2)//if dip switch 2 turn on all cled off.
        {
            if(preDip != dip)//if dip switch value is same with preview dip switch value, nothing.
            {
                cledContr(fd, 0, 0, 0, 0);
                cledContr(fd, 1, 0, 0, 0);
                cledContr(fd, 2, 0, 0, 0);
                cledContr(fd, 3, 0, 0, 0);
            }
            preDip = dip;//preview dip switch value
        }
        //dip switch 2 turn off, cled can turn on.
        else if( liq_prev != shared->liq_exist ) {//if liquid height is same preview liquid height, nothing.
            if (!shared->liq_exist) {//liquid height is enough, cled0 turn off
                cledContr(fd, 0, 0, 0, 0);
            }
            else {//liquid height is not enough, cled0 turn on with blue
                cledContr(fd, 0, 0, 0, 255);
            }

        }
        liq_prev = shared->liq_exist;
        usleep(1000000);
    /*
        if () {
            cledContr(fd, 1, 0, 255, 0);
            cledContr(fd, 1, 0, 0, 0);
        }
        */
/*
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

    }
    close(fd);

    return;
}

void* cled(void* shared) {
    _cled((Shared*)shared);//for use to thread.
}