/*
	@file	-	buzzer.cpp
	@author	-	
	@brief	-	control buzzer
	@reference -	/root/periApp/buzzer/buzzertest.c
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
#include "buzzer.h"
#include "dipsw.h"
#include "thread_manager.h"

#define DRIVER_BUZZER			"/dev/cnbuzzer" ///< buzzer file driver
#define MAX_BUZZER_NUMBER		36 ///< maximum number of sound type of buzzer.

//do(1,13,25,37) ,do#(2,14,26)
//re(3,15,27), re#(4,16,28)
//mi(5,17,29)
//fa(6,18,30), fa#(7,19,31)
//sol(8,20,32), sol#(9,21,33)
//ra(10,22,34), ra#(11,23,35)
//si(12,24,36)  
//control buzzer sound, alarm for 100000 micro second.
void buzzer(int buzzerNumber)
{
	int fd;

	fd = open(DRIVER_BUZZER, O_RDWR);///< open driver
	if ( fd < 0 )
	{
		perror("driver (//dev//cnbuzzer) open error.\n");
		return;
	}
	pthread_mutex_lock(thread_manager::get_buzz());///< waiting for priting picture of oled.
	write(fd, &buzzerNumber, 4);
	pthread_mutex_unlock(thread_manager::get_buzz());

	usleep(100000);///< waiting 100000 micro second.
    	buzzerNumber = 0;

	pthread_mutex_lock(thread_manager::get_buzz());///< waiting for priting picture of oled.
	write(fd, &buzzerNumber, 4);
	pthread_mutex_unlock(thread_manager::get_buzz());

    	close(fd);///< close file drvier.
	
	return;
}

void dip_buzzer(const int arr[], const int arr_len)
{
	int i;

	int dips = dipsw();///< load dip switch. 

	//if first dip switch turn on, buzzer sound off.
	if((dips & 1) != 1)
	{
		// if first dip switch turn off, sequentially alarm the buzzer.
		for(i = 0 ; i < arr_len ; i++)
		{
			buzzer(arr[i]);
		}
	}
	return;
}
