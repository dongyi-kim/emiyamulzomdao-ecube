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

#define DRIVER_SEGMENT		"/dev/cnfnd"

#define MAX_FND_NUM		6

#define  DOT_OR_DATA	0x80

const unsigned short segNum[11] =
        {
                0x3F, // 0
                0x06,
                0x5B,
                0x4F,
                0x66,
                0x6D,
                0x7D,
                0x27,
                0x7F,
                0x6F, // 9
                0x00
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
    if( dir == 1)
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

#define ONE_SEG_DISPLAY_TIME_USEC	1000
// return 1 => exit  , 0 => success
int fndDisp(int driverfile, int* num , int dotflag)
{

    int cSelCounter,loopCounter;
    int temp, i, totalCount;
    unsigned short wdata;
    int dotEnable[MAX_FND_NUM];
    int fndChar[MAX_FND_NUM] = {0, 0, 0, 0, 0, 0};

    for (i = 0; i < MAX_FND_NUM ; i++ )
    {
        dotEnable[i] = dotflag & (0x1 << i);
    }

    cout<<(*num)<<endl;
    // if 6 fnd
    while(1) {

        if( (*num) == -1 ) {
            for(i = 5 ; i >= 0 ; i--)
            {
                fndChar[i] = 10;
            }
        }

        else {
            temp = (*num)%1000000;

            if( temp == -1 ) temp = 0;
            for(i = 5 ; i >= 0 ; i--)
            {
                fndChar[i] = temp%10;
                temp /= 10;
            }
        }
        int duration = 1;
        while(duration--) {
            cSelCounter = 0;
            while (cSelCounter < MAX_FND_NUM) {
                wdata = segNum[fndChar[cSelCounter]] | segSelMask[cSelCounter];
                if (dotEnable[cSelCounter])
                    wdata |= DOT_OR_DATA;
                pthread_mutex_lock(thread_manager::get_seg());
                write(driverfile, &wdata, 2);
                pthread_mutex_unlock(thread_manager::get_seg());
                cSelCounter++;


                usleep(ONE_SEG_DISPLAY_TIME_USEC);

            }
        }
        wdata = 0;
        pthread_mutex_lock(thread_manager::get_seg());
        write(driverfile, &wdata, 2);
        pthread_mutex_unlock(thread_manager::get_seg());
    }

    return 1;
}

void segment(Shared* shared)
{
    int fd;

    // open  driver
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