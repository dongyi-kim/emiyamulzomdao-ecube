/*
        @file   -       fnd.cpp
        @author -       
        @brief  -       control 7-segment
        @reference -    /root/periApp/fndtest/fndtest.c(system programming practice 7th device driver.pptx)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "thread_manager.h"

#define DRIVER_SEGMENT		"/dev/cnfnd"///< 7-segment file driver path

#define MAX_FND_NUM		6///< the maximum number of 7-segment.

#define  DOT_OR_DATA	0x80

const unsigned short segNum[11] =
        {
                0x3F, ///< 0
                0x06, ///< 1
                0x5B, ///< 2
                0x4F, ///< 3
                0x66, ///< 4
                0x6D, ///< 5
                0x7D, ///< 6
                0x27, ///< 7
                0x7F, ///< 8
                0x6F, ///< 9
                0x00 ///< off
        };
const unsigned short segSelMask[MAX_FND_NUM] =
        {
                0xFE00,
                0xFD00,
                0xFB00,
                0xF700,
                0xEF00,
                0xDF00
        };
static struct termios oldt, newt;

void fnd_changemode(int dir)
{
    if(dir == 1)
    {
        tcgetattr(STDIN_FILENO , &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO );
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    else
    {
        tcsetattr(STDIN_FILENO , TCSANOW, &oldt);

    }
}

#define ONE_SEG_DISPLAY_TIME_USEC	1000 ///< display time each 7-segment

/**
display control 7-segment
*/
int fndDisp(int driverfile, int* num , int dotflag)
{
    int cSelCounter;///< each 7-segment index
    int temp;///< temporary variable.
    unsigned short wdata;
    int dotEnable[MAX_FND_NUM];
    int fndChar[MAX_FND_NUM] = {0, 0, 0, 0, 0, 0};///< each 7-segment value.
    
    //dot express
    for (i = 0; i < MAX_FND_NUM ; i++ )
    {
        dotEnable[i] = dotflag & (0x1 << i);
    }
    //7-segment display
    while(1) {
        //if segment data is -1, initialize
        if( (*num) == -1 ) {
            for(i = 5 ; i >= 0 ; i--)
            {
                fndChar[i] = 10;
            }
        }
        //else each digit push fndChar
        else {
            temp = (*num)%1000000;

            if( temp == -1 ) temp = 0;
            for(i = 5 ; i >= 0 ; i--)
            {
                fndChar[i] = temp%10;
                temp /= 10;
            }
        }
        cSelCounter = 0;
        //display 7-segment value
        while (cSelCounter < MAX_FND_NUM) {
                wdata = segNum[fndChar[cSelCounter]] | segSelMask[cSelCounter];
                if (dotEnable[cSelCounter])
                    wdata |= DOT_OR_DATA;
                pthread_mutex_lock(thread_manager::get_seg());///< waiting for printing picture of oled.
                write(driverfile, &wdata, 2);
                pthread_mutex_unlock(thread_manager::get_seg());
                cSelCounter++;
                usleep(ONE_SEG_DISPLAY_TIME_USEC);///< display time
        }
        wdata = 0;
        pthread_mutex_lock(thread_manager::get_seg());///< waiting for printing picture of oled.
        write(driverfile, &wdata, 2);
        pthread_mutex_unlock(thread_manager::get_seg());
    }

    return 1;
}
/**
call the 7-segment display function.
*/
void segment(Shared* shared)
{
    int fd;
    // open 7-segment file driver
    fd = open(DRIVER_SEGMENT, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver open error.\n");
        return;
    }
    fnd_changemode(1);

    fndDisp(fd, &shared->segValue, 0);

    fnd_changemode(0);
    close(fd);
    return;
}

void* fnd(void* shared) {
    segment((Shared*)shared);
}
