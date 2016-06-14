/*
	@file	-	cled.cpp
	@author	-	
	@brief	-	control full color led
	@reference -	system programming([Practice]7th_Device driver.pptx)
*/

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

#define DRIVER_CLED		"/dev/cncled" ///< full color led file drvice path.

#define INDEX_LED		0
#define INDEX_REG_LED		1
#define INDEX_GREEN_LED		2
#define INDEX_BLUE_LED		3
#define INDEX_MAX		4

/**
color range 0 ~ 255
the number of led is 4, index 0 ~ 3
Red, Green, Blue 0~255, 0~255, 0~255
contol full color led turn on or turn off
*/
void cledContr(int driverfile, int idx_led, int r, int g, int b)
{
    unsigned short colorArray[INDEX_MAX];
    colorArray[INDEX_LED] = idx_led;///< colorArray[0] is led index of want contorl.
    colorArray[INDEX_REG_LED] =(unsigned short) r;///< colorArray[1] is red color value 0~255.
    colorArray[INDEX_GREEN_LED] =(unsigned short) g;///< colorArray[2] is green color value 0~255.
    colorArray[INDEX_BLUE_LED] =(unsigned short) b;///< colorArray[3] is blue color value 0~255.

    pthread_mutex_lock(thread_manager::get_cled());///< waiting of printing picture of oled.
    write(driverfile, &colorArray ,6);
    pthread_mutex_unlock(thread_manager::get_cled());

    return;
}


void _cled(Shared* shared)
{
    int fd;
    int dip;
    int preDip = -1;
    // open oled file driver
    fd = open(DRIVER_CLED, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver  open error.\n");
        return;
    }

    bool liq_prev = !shared->liq_exist;

    while(1) {
        dip = dipsw();//load dip switch data.
        
        //if dip second dipswitch turn on, all full color led off.
        if(dip & 2)
        {
            //do not repeat the turn off operation.
            if(preDip != dip)
            {
                cledContr(fd, 0, 0, 0, 0);///<0th led off
                cledContr(fd, 1, 0, 0, 0);///<1th led off
                cledContr(fd, 2, 0, 0, 0);///<2th led off
                cledContr(fd, 3, 0, 0, 0);///<3th led off
            }
            preDip = dip;
            usleep(1000000);
            continue;
        }
        //for do not repeat the turn on or turn off the led, check preview value.
        if( liq_prev != shared->liq_exist ) {
       	    //if liquid height sensor detect the liquid, 0th led turn off
            if (!shared->liq_exist) {
                cledContr(fd, 0, 0, 0, 0);
            }
            //if liquid height sensor do not detect the liquid, 0th led turn on by blue 255 color.
            else {
                cledContr(fd, 0, 0, 0, 255);
            }

        }
        liq_prev = shared->liq_exist;
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
    _cled((Shared*)shared);
}
