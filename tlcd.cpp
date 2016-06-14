/**
	@file	-	tlcd.cpp
	@author	-	
	@brief	-	write text on text lcd
	@reference - system programming([Practice]8th_Device_Driver.pptx)
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include "common.h"
#include "thread_manager.h"

using namespace std;

static int  fd ;

#define DRIVER_TLCD		"/dev/cntlcd"///< text LCD file driver path.
#define CLEAR_DISPLAY		0x0001
#define CURSOR_AT_HOME		0x0002

// Entry Mode set
#define MODE_SET_DEF		0x0004
#define MODE_SET_DIR_RIGHT	0x0002
#define MODE_SET_SHIFT		0x0001

// Display on off
#define DIS_DEF				0x0008
#define DIS_LCD				0x0004
#define DIS_CURSOR			0x0002
#define DIS_CUR_BLINK		0x0001

// shift
#define CUR_DIS_DEF			0x0010
#define CUR_DIS_SHIFT		0x0008
#define CUR_DIS_DIR			0x0004

// set DDRAM  address
#define SET_DDRAM_ADD_DEF	0x0080

// read bit
#define BUSY_BIT			0x0080
#define DDRAM_ADD_MASK		0x007F


#define DDRAM_ADDR_LINE_1	0x0000
#define DDRAM_ADDR_LINE_2	0x0040


#define SIG_BIT_E			0x0400
#define SIG_BIT_RW			0x0200
#define SIG_BIT_RS			0x0100

int IsBusy(void)
{
    unsigned short wdata, rdata;

    wdata = SIG_BIT_RW;
    pthread_mutex_lock(thread_manager::get_tlcd());
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    wdata = SIG_BIT_RW | SIG_BIT_E;

    pthread_mutex_lock(thread_manager::get_tlcd());
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    pthread_mutex_lock(thread_manager::get_tlcd());
    read(fd,&rdata ,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    wdata = SIG_BIT_RW;
    pthread_mutex_lock(thread_manager::get_tlcd());
    write(fd,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    if (rdata &  BUSY_BIT)
        return 1;

    return 0;
}

int tlcd_writeCmd(unsigned short cmd)
{
    unsigned short wdata ;

    if ( IsBusy())
        return 0;

    wdata = cmd;
    pthread_mutex_lock(thread_manager::get_tlcd());
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    wdata = cmd | SIG_BIT_E;
    pthread_mutex_lock(thread_manager::get_tlcd());
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    wdata = cmd ;
    pthread_mutex_lock(thread_manager::get_tlcd());
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    return 1;
}

//set start print text.
int setDDRAMAddr(int x , int y)
{
    unsigned short cmd = 0;
    
    if(IsBusy())
    {
        perror("setDDRAMAddr busy error.\n");
        return 0;
    }

    if ( y == 1 )
    {
        cmd = DDRAM_ADDR_LINE_1 +x;
    }
    else if(y == 2 )
    {
        cmd = DDRAM_ADDR_LINE_2 +x;
    }
    else
        return 0;
    if ( cmd >= 0x80)
        return 0;
    if (!tlcd_writeCmd(cmd | SET_DDRAM_ADD_DEF))
    {
        perror("setDDRAMAddr error\n");
        return 0;
    }
    usleep(1000);
    return 1;
}

int displayMode(int bCursor, int bCursorblink, int blcd  )
{
    unsigned short cmd  = 0;

    if ( bCursor)
    {
        cmd = DIS_CURSOR;
    }

    if (bCursorblink )
    {
        cmd |= DIS_CUR_BLINK;
    }

    if ( blcd )
    {
        cmd |= DIS_LCD;
    }

    if (!tlcd_writeCmd(cmd | DIS_DEF))
        return 0;

    return 1;
}

int writeCh(unsigned short ch)
{
    unsigned short wdata =0;

    if ( IsBusy())
        return 0;

    wdata = SIG_BIT_RS | ch;
    pthread_mutex_lock(thread_manager::get_tlcd());///< waiting for printing picture of oled
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    wdata = SIG_BIT_RS | ch | SIG_BIT_E;
    pthread_mutex_lock(thread_manager::get_tlcd());///< waiting for printing picture of oled
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());

    wdata = SIG_BIT_RS | ch;
    pthread_mutex_lock(thread_manager::get_tlcd());///< waiting for printing picture of oled
    write(fd ,&wdata,2);
    pthread_mutex_unlock(thread_manager::get_tlcd());
    usleep(1000);
    return 1;

}
//print blink cursor
int setCursorMode(int bMove , int bRightDir)
{
    unsigned short cmd = MODE_SET_DEF;

    if (bMove)
        cmd |=  MODE_SET_SHIFT;

    if (bRightDir)
        cmd |= MODE_SET_DIR_RIGHT;

    if (!tlcd_writeCmd(cmd))
        return 0;
    return 1;
}

int functionSet(void)
{
    unsigned short cmd = 0x0038; // 5*8 dot charater , 8bit interface , 2 line

    if (!tlcd_writeCmd(cmd))
        return 0;
    return 1;
}
//print stirng on text lcd
int writeStr(const char* str)
{
    unsigned char wdata;
    int i;
    for(i =0; i < strlen(str) ;i++ )
    {
        if (str[i] == '_')
            wdata = (unsigned char)' ';
        else
            wdata = str[i];
        writeCh(wdata);
    }
    return 2;

}

#define LINE_NUM			2
#define COLUMN_NUM			16
#define CMD_TXT_WRITE		0
#define CMD_CURSOR_POS		1
#define CMD_CEAR_SCREEN		2

//make string for print on lcd.
string make_str(int value, char c)
{
    int temp = 100000;///< 7-segment maximum digit place is six.

    string str = "";

    str+=c;///< c is L(iLlumination), T(Temperature), H(Tumidity), S(Soil humidity)
    //text will be expressed C000XXX, ex) L000053
    for(int i = 0 ; i < 6 ; i++) {
    
        str += (value/temp+'0');

        value %= temp;

        temp/=10;
    }

    return str;
}

//print text on text lcd
void _tlcd(Shared* shared) {

    int light;
    int temp;
    int humid;
    int soil_humid;

    fd = open(DRIVER_TLCD, O_RDWR);//open text lcd file driver.
    if ( fd < 0 )
    {
        perror("driver open error.\n");
        return;
    }

    string prev_above = "-1";
    string prev_under = "-1";

    functionSet();
    displayMode(1, 1, 1);///< blink cursor

    while(1)
    {
        string above = "";///< text for above line
        string under = "";///< text for under line

        if(shared->mode == OBSERVE_MODE)///< in observe mode, text lcd show sensor data value.
        {
            light = shared->sensor.illumination;
            temp = shared->sensor.temperature;
            humid = shared->sensor.humidity;
            soil_humid = shared->sensor.soil_humidity;
        }

        else if(shared->mode == EDIT_MODE) {///< in edit mode, text lcd show standard data value.
            light = shared->data.illumination;
            temp = shared->data.temperature;
            humid = shared->data.humidity;
            soil_humid = shared->data.soil_humidity;
        }
	//above text is illumination and temperature data.
        above += make_str(light, 'L');
        above += ' ';
        above += make_str(temp, 'T');
        above += ' ';
        //under text is humidity and soil humidity data.
        under += make_str(humid, 'H');
        under += ' ';
        under += make_str(soil_humid, 'S');
        under += ' ';
        //do not repeat the print text.
        if(prev_above == above && prev_under == under) {
            continue;
        }

        else
        {
            setDDRAMAddr(0,1);
            writeStr(above.c_str());//print text above line.
            setDDRAMAddr(0,2);
            writeStr(under.c_str());//print text under line
        }

        prev_above = above;
        prev_under = under;
        
        usleep(100000);
    }
    close(fd);
    return;
}

void* tlcd(void* shared) {
    _tlcd((Shared*)shared);
}
