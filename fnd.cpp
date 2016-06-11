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

#define DRIVER_SEGMENT		"/dev/cnfnd"

#define MAX_FND_NUM		6

#define  DOT_OR_DATA	0x80

const unsigned short segNum[10] =
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
                0x6F  // 9
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
    // if 6 fnd
    while(1) {
        temp = (*num)%1000000;

        if( temp == -1 ) temp = 0;
        for(i = 5 ; i >= 0 ; i--)
        {
            fndChar[i] = temp%10;
            temp /= 10;
        }

        cSelCounter = 0;
        while(cSelCounter < MAX_FND_NUM)
        {
            wdata = segNum[fndChar[cSelCounter]]  | segSelMask[cSelCounter] ;
            if (dotEnable[cSelCounter])
                wdata |= DOT_OR_DATA;

            write(driverfile,&wdata,2);
            cSelCounter++;


            usleep(ONE_SEG_DISPLAY_TIME_USEC);

        }
        wdata = 0;
        write(driverfile, &wdata, 2);
        usleep(500);
    }

    return 1;
}

void segment(int* number)
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
    fndDisp(fd, number, 0);
    fnd_changemode(0);
    close(fd);

    return;
}

void* fnd(void* number) {
    segment((int*)number);
}