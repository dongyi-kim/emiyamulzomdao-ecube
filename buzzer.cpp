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
#include "buzzer.h"
#include "dipsw.h"
#include "thread_manager.h"

#define DRIVER_BUZZER			"/dev/cnbuzzer"
#define MAX_BUZZER_NUMBER		36

void buzzer(int buzzerNumber)
{
	int fd;

	// open  driver 
	fd = open(DRIVER_BUZZER, O_RDWR);
	if ( fd < 0 )
	{
		perror("driver (//dev//cnbuzzer) open error.\n");
		return;
	}
	// control led
	pthread_mutex_lock(thread_manager::get_buzz());//waiting print picture complete in oled
	write(fd, &buzzerNumber, 4);
	pthread_mutex_unlock(thread_manager::get_buzz());

	usleep(100000);
    buzzerNumber = 0;

	pthread_mutex_lock(thread_manager::get_buzz());//waiting print picture complete in oled
	write(fd, &buzzerNumber, 4);
	pthread_mutex_unlock(thread_manager::get_buzz());

    close(fd);
	
	return;
}

void dip_buzzer(const int arr[], const int arr_len)
{
	int dips = dipsw();//load dip switch value

	if((dips & 1) != 1)//if dip switch 1 turn on, buzzer not alarm.
	{
		for(int i = 0 ; i < arr_len ; i++)
		{
			buzzer(arr[i]);
		}
	}
	return;
}