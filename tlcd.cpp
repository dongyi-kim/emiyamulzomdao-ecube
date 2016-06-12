//
// Created by root on 16. 6. 12.
//

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

using namespace std;

static int  fd ;

#define DRIVER_TLCD		"/dev/cntlcd"
/******************************************************************************
*
*      TEXT LCD FUNCTION
*
******************************************************************************/
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

/***************************************************
read /write  sequence
write cycle
RS,(R/W) => E (rise) => Data => E (fall)

***************************************************/
int IsBusy(void)
{
    unsigned short wdata, rdata;

    wdata = SIG_BIT_RW;
    write(fd ,&wdata,2);

    wdata = SIG_BIT_RW | SIG_BIT_E;
    write(fd ,&wdata,2);

    read(fd,&rdata ,2);

    wdata = SIG_BIT_RW;
    write(fd,&wdata,2);

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
    write(fd ,&wdata,2);

    wdata = cmd | SIG_BIT_E;
    write(fd ,&wdata,2);

    wdata = cmd ;
    write(fd ,&wdata,2);

    return 1;
}

int setDDRAMAddr(int x , int y)
{
    unsigned short cmd = 0;
//	printf("x :%d , y:%d \n",x,y);
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


//	printf("setDDRAMAddr w1 :0x%X\n",cmd);

    if (!tlcd_writeCmd(cmd | SET_DDRAM_ADD_DEF))
    {
        perror("setDDRAMAddr error\n");
        return 0;
    }
//	printf("setDDRAMAddr w :0x%X\n",cmd|SET_DDRAM_ADD_DEF);
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
    write(fd ,&wdata,2);

    wdata = SIG_BIT_RS | ch | SIG_BIT_E;
    write(fd ,&wdata,2);

    wdata = SIG_BIT_RS | ch;
    write(fd ,&wdata,2);
    usleep(1000);
    return 1;

}


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

int writeStr(char* str)
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
int clearScreen(int nline)
{
    int i;
    if (nline == 0)
    {
        if(IsBusy())
        {
            perror("clearScreen error\n");
            return 0;
        }
        if (!tlcd_writeCmd(CLEAR_DISPLAY))
            return 0;
        return 1;
    }
    else if (nline == 1)
    {
        setDDRAMAddr(0,1);
        for(i = 0; i <= COLUMN_NUM ;i++ )
        {
            writeCh((unsigned char)' ');
        }
        setDDRAMAddr(0,1);

    }
    else if (nline == 2)
    {
        setDDRAMAddr(0,2);
        for(i = 0; i <= COLUMN_NUM ;i++ )
        {
            writeCh((unsigned char)' ');
        }
        setDDRAMAddr(0,2);
    }
    return 1;
}

#define CMD_TXT_WRITE		0
#define CMD_CURSOR_POS		1
#define CMD_CEAR_SCREEN		2

/*
void doHelp(void)
{
	printf("Usage:\n");
	printf("tlcdtest w line string :=>display the string  at line  , charater  '_' =>' '\n");
	printf(" ex) tlcdtest w 0 cndi_text_test :=>display 'cndi text test' at line 1 \n");
	printf("tlcdtest c on|off blink line column : \n");
	printf(" => cursor set on|off =>1 or 0 , b => blink 1|0 , line column line position \n");
	printf("tlcdtset c  1 1 2 12  :=> display blink cursor at 2 line , 12 column.\n");
	printf("tlcdtest r [line] : => clear screen or clear line \n");
	printf("tlcdtest r  : => clear screen \n");
	printf("tlcdtest r 1: => clear line 1 \n");
}
*/
void tlcd(char str[])
{
    int nCmdMode;
    int bCursorOn, bBlink, nline , nColumn;

    // open  driver




    //setDDRAMAddr(0, 1);


    /*
    switch ( nCmdMode )
    {
    case CMD_TXT_WRITE:
//		printf("nline:%d ,nColumn:%d\n",nline,nColumn);
        setDDRAMAddr(nColumn, nline);
        usleep(2000);
        writeStr(strWtext);
        break;
    case CMD_CURSOR_POS:
        displayMode(bCursorOn, bBlink, TRUE);
        setDDRAMAddr(nline-1, nColumn);
        break;
    case CMD_CEAR_SCREEN:
        clearScreen(nline);
        break;
    }
    */

    close(fd);

    return;
}

string make_str(int value, char c)
{
    string str;
    str += c;
    if(value == 0)
    {
        l += '0';
    }
    else
    {
        while(value)
        {
            str += '0'+value/10;
            value/10;
        }
    }
    return str;
}


void _tlcd(Shared* shared) {

    fd = open(DRIVER_TLCD, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver open error.\n");
        return;
    }
    while(1)
    {
        int light = shared->sensor.illumination;
        int temp = shared->sensor.temperature;
        int humid = shared->sensor.humidity;
        int soil_humid = shared->sensor.soil_humidity;
        string above;
        string under;
        above += make_str(light, 'L');
        above += ' ';
        above += make_str(temp, 'T');
        under += make_str(humid, 'H');
        under += ' ';
        under += make_str(soil_humid, 'S');

        functionSet();
        displayMode(1, 1, 1);
        clearScreen(2);
        clearScreen(1);
        writeStr(above.c_str());
        setDDRAMAddr(0,2);
        writeStr(under.c_str());
    }
    close(fd);
    retrun;
}

void* tlcd(void* shared) {
    _tlcd((Shared*)shared);
}