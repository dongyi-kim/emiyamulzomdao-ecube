/*
    @file   -   bled.cpp
    @author -   
    @brief  -   control bus led
    @reference - /root/periApp/ledtest/ledtest.c
*/
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

#define MAX_LED_NO		8///< maximum number of bus led.
#define DRIVER_BUSLED		"/dev/cnled"///< bus led file driver.
//bus led control
void ledContr(int driverfile, Data* s, Data* d)
{
    int ledOn = 8;///< number of turn on led
    int ledOff = 0;///< number of turn off led
    int wdata, rdata, temp ;

    int std_sh = d->soil_humidity;
    int sh = s->soil_humidity;

    //soil humidity sensor normal value is 300~400
    if(sh >= 200 && sh <= 250)///< when soil humidity sensor in water, the sensor value is 200 ~ 250
    {
        ledOff = 0;
    }
    else if(sh >= 500 && sh <= 550)///< when soil humidty sensor in dry sand, the sensor value is 500~550
    {
        ledOff = 8;
    }
    else if(std_sh < sh)///< when standard soil humidity greater than sensor value, water not enough
    {
        while(ledOff < 8 && sh > std_sh)
        {
            sh -= 15;
            ledOff++;
        }
    }

    // control bus led
    ledOn = MAX_LED_NO - ledOff;
    for(int i = 0 ; i < MAX_LED_NO ; i++)
    {
        pthread_mutex_lock(thread_manager::get_bled());///< waiting for printing picture of oled
        read(driverfile, &rdata, 4);
        pthread_mutex_unlock(thread_manager::get_bled());
        temp = 1;
        if(i >= ledOn)///< turn off led
        {
            temp = ~(temp<<(i));
            wdata = rdata & temp;
        }
        else///< turn on led
        {
            temp <<= (i);
            wdata = rdata | temp;
        }
        pthread_mutex_lock(thread_manager::get_bled());///< waiting for printing picture of oled
        write(driverfile, &wdata, 4);
        pthread_mutex_unlock(thread_manager::get_bled());
    }

    return;
}

void _bled(Shared* shared)
{
    int fd;

    // open bus led file driver
    fd = open(DRIVER_BUSLED, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver open error.\n");
        return;
    }
    while(1) {
        ledContr(fd, &shared->sensor, &shared->data);
    }
    close(fd);

    return;
}

void* bled(void* shared) {
    _bled((Shared*)shared);
}
