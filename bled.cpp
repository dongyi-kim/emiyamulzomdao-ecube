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
#include "thread_manager.h"

using namespace std;

#define MAX_LED_NO		8
#define DRIVER_BUSLED		"/dev/cnled"

void ledContr(int driverfile, Data* s, Data* d)
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
    if(sh == 0)
    {
        ledOff = 8;
    }


    // control led
    ledOn = MAX_LED_NO - ledOff;
    for(int i = 0 ; i < MAX_LED_NO ; i++)
    {
        pthread_mutex_lock(thread_manager::get_bled());
        read(driverfile, &rdata, 4);
        pthread_mutex_unlock(thread_manager::get_bled());
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
        pthread_mutex_lock(thread_manager::get_bled());
        write(driverfile, &wdata, 4);
        pthread_mutex_unlock(thread_manager::get_bled());
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
        ledContr(fd, &shared->sensor, &shared->data);
        usleep(100000);

    }
    close(fd);

    return;
}

void* bled(void* shared) {
    _bled((Shared*)shared);
}
