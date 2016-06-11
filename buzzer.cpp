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
	
	write(fd, &buzzerNumber, 4);
	usleep(100000);
    buzzerNumber = 0;
	write(fd, &buzzerNumber, 4);
    close(fd);
	
	return;
}
