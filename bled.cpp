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
    //led show sensor's soil humidity compare with plant's standard value.
    int ledOn = 8;
    int ledOff = 0;
    int wdata, rdata, temp ;

    int std_sh = d->soil_humidity;//plant's standard value
    int sh = s->soil_humidity;//value of sensor measure

    if(std_sh < sh)//soil humid sensor has reverse value, smaller value more humid
    {//sensor value greater than standard value ledOff bigger;
        ledOff = (sh-std_sh)/10;
        if(ledOff > 8 || (sh >= 500 && sh <= 550))//soil humid sensor in water has 200~250 value. maximum dry value has 500~550 value.
        {
            ledOff = 8;
        }
        else if(sh >= 200 && sh <= 250)
        {
            ledOff = 0;
        }
    }

    // control led
    ledOn = MAX_LED_NO - ledOff;
    for(int i = 0 ; i < MAX_LED_NO ; i++)
    {
        pthread_mutex_lock(thread_manager::get_bled());//waiting print picture complete in oled
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
        pthread_mutex_lock(thread_manager::get_bled());//waiting print picture complete in oled
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
        ledContr(fd, &shared->sensor, &shared->data);//led control
    }
    close(fd);

    return;
}

void* bled(void* shared) {
    _bled((Shared*)shared);//for use to thread
}
