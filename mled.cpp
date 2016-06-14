/*
	@file	-	mled.cpp
	@author	-	
	@brief	-	control dot matrix led
	@reference -	/root/periApp/mledtest/mledtest.c(system programming practice 7th device driver.pptx)
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
#include <termios.h>
#include "common.h"
#include "thread_manager.h"

#define DRIVER_MLED		"/dev/cnmled"///< dot matrix led file driver path.

#define MAX_COLUMN_NUM	5 ///< dot matrix 7(row)*5(col)

const unsigned short NumData[10][MAX_COLUMN_NUM]=
        {
                {0xfe00,0xfd7F,0xfb41,0xf77F,0xef00}, // 0
                {0xfe00,0xfd42,0xfb7F,0xf740,0xef00}, // 1
                {0xfe00,0xfd79,0xfb49,0xf74F,0xef00}, // 2
                {0xfe00,0xfd49,0xfb49,0xf77F,0xef00}, // 3
                {0xfe00,0xfd0F,0xfb08,0xf77F,0xef00}, // 4
                {0xfe00,0xfd4F,0xfb49,0xf779,0xef00}, // 5
                {0xfe00,0xfd7F,0xfb49,0xf779,0xef00}, // 6
                {0xfe00,0xfd07,0xfb01,0xf77F,0xef00}, // 7
                {0xfe00,0xfd7F,0xfb49,0xf77F,0xef00}, // 8
                {0xfe00,0xfd4F,0xfb49,0xf77F,0xef00}  // 9
        };
//when edit mode show selected menu to alphabet.
const unsigned short config[4][2][MAX_COLUMN_NUM] = {
        {
                {0xfe00, 0xfd41, 0xfb7F, 0xf741, 0xef00}, //I
                {0xfe7F, 0xfd40, 0xfb40, 0xf740, 0xef40}  //L
        },
        {
                {0xfe01, 0xfd01, 0xfb7F, 0xf701, 0xef01}, //T
                {0xfe00, 0xfd7F, 0xfb05, 0xf705, 0xef07}  //P
        },
        {
                {0xfe7F, 0xfd08, 0xfb08, 0xf708, 0xef7F}, //H
                {0xfe7F, 0xfd02, 0xfb04, 0xf702, 0xef7F}  //M
        },
        {
                {0xfe4F, 0xfd49, 0xfb49, 0xf779, 0xef00}, //S
                {0xfe7F, 0xfd08, 0xfb08, 0xf708, 0xef7F}  //H
        }
};
//turn off 7-segment
const unsigned short clear[2][MAX_COLUMN_NUM] =
        {
                {0xfe00, 0xfe00, 0xfe00, 0xfe00, 0xfe00},
                {0xfe00, 0xfe00, 0xfe00, 0xfe00, 0xfe00}
        };

static struct termios oldt, newt;
void mled_changemode(int dir)
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

#define ONE_LINE_TIME_U 	1000///< show time of each column 
/**
control display dot matrix led
*/
void displayDotLed(int driverfile, State* s)
{
    int i, j;
    unsigned short wdata[2] = {0, 0};//index[0] is left dot matrix led, index[1] is right dot matrix led

    int _state;///< selected menu

    bool init_flag = true;

    while(1)
    {
        //if len is 0(not select menu), initialize dot matrix led
        if( s->len == 0 && init_flag ) {
            for(j = 0 ; j < MAX_COLUMN_NUM ; j++)
            {
                wdata[0] = clear[0][j];///< turn off left dot matrix led
                wdata[1] = clear[1][j];///< turn off right dot matrix led

                pthread_mutex_lock(thread_manager::get_mled());///< waiting for printing picture of oled.
                write(driverfile,(unsigned char*)wdata, 4);
                pthread_mutex_unlock(thread_manager::get_mled());
                usleep(ONE_LINE_TIME_U);
            }

            init_flag = false;
        }
        //display dot matrix led, selected menu.
        else {
            for(i = 0 ; i < s->len ; i++)
            {
                _state = s->state[i];//selected menu
                //turn on left and right dot matrix led by selected menu(IL:illumination, TP:temperature, HD:humidity, SH:soil humidity)
                for(j = 0 ; j < MAX_COLUMN_NUM ; j++)
                {
                    if( 1 <= _state && _state <= 4 ) {
                        wdata[0] = config[_state-1][0][j];
                        wdata[1] = config[_state-1][1][j];
                    }
                    pthread_mutex_lock(thread_manager::get_mled());///< waiting for printing picture of oled.
                    write(driverfile,(unsigned char*)wdata, 4);
                    pthread_mutex_unlock(thread_manager::get_mled());
                    usleep(ONE_LINE_TIME_U);
                }
            }
            init_flag = true;
        }
    }
    wdata[0]= 0;
    wdata[1]= 0;
    write(driverfile,(unsigned char*)wdata,4);

    return;
}

void _mled(Shared* s)
{
    int fd;

    mled_changemode(1);
    
    fd = open(DRIVER_MLED, O_RDWR);///< open dot matrix led file driver
    if ( fd < 0 )
    {
        perror("driver  open error.\n");
        return;
    }
    displayDotLed(fd, &(s->state));

    mled_changemode(0);
    close(fd);

    return;
}

void* mled(void* s)
{
    _mled((Shared*)s);
}
