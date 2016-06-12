//
// Created by root on 16. 6. 12.
//
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"

using namespace std;

#define MAX_LED_NO		8
#define DRIVER_BUSLED		"/dev/cnled"

void ledContr(int driverfile, bool off, Data* s, Data* d)
{
    int ledOn = 8;
    int ledOff = 0;
    int wdata, rdata, temp ;

    int std_sh = d->soil_humidity;
    int sh = s->soil_humidity;

    if(std_sh < sh)
    {
        ledOff = (sh-std_sh)/70;
    }
    if(off)
    {
        ledOff = 8;
    }

    // control led
    ledOn = MAX_LED_NO - ledOff;
    for(int i = 1 ; i < MAX_LED_NO ; i++)
    {
        read(driverfile, &rdata, 4);
        temp = 1;
        if(i >= ledOn)
        {
            temp = ~(temp<<(i));
            wdata = rdata & temp;
        }
        else
        {
            temp <<= (i);
            wdata = rdata | temp;
        }
        write(driverfile, &wdata, 4);
    }

    return;
}

void _bled(Shared* shared)
{
    int fd;

    // open  driver
    fd = open(DRIVER_BUSLED, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver open error.\n");
        return;
    }
    while(1) {
        ledContr(fd, &shared->Off[2], &shared->sensor, &shared->data);
        usleep(5000);
    }
    close(fd);

    return;
}

void* bled(void* shared) {
    _bled((Shared*)shared);
}
