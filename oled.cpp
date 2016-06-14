/*
        @file   -       fnd.cpp
        @author -       
        @brief  -       print picture on oled.
        @reference -    system programming([Practice]8th_Device driver.pptx)
*/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "common.h"
#include "thread_manager.h"

using namespace std;

#define DRIVER_OLED		"/dev/cnoled" ///< oled file driver path.

static  int  fd ;

unsigned long simple_strtoul(char *cp, char **endp,unsigned int base)
{
    unsigned long result = 0,value;

    if (*cp == '0') {
        cp++;
        if ((*cp == 'x') && isxdigit(cp[1])) {
            base = 16;
            cp++;
        }
        if (!base) {
            base = 8;
        }
    }
    if (!base) {
        base = 10;
    }
    while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp) ? toupper(*cp) : *cp)-'A'+10) < base) {
        result = result*base + value;
        cp++;
    }
    if (endp)
        *endp = (char *)cp;
    return result;
}

unsigned long read_hex(const char* str){
    char addr[128];
    strcpy(addr,str);
    return simple_strtoul(addr, NULL, 16);
}


#define RST_BIT_MASK	0xEFFF
#define CS_BIT_MASK		0xF7FF
#define DC_BIT_MASK		0xFBFF
#define WD_BIT_MASK		0xFDFF
#define RD_BIT_MASK		0xFEFF
#define DEFAULT_MASK	0xFFFF


#define CMD_SET_COLUMN_ADDR		0x15
#define CMD_SET_ROW_ADDR		0x75
#define CMD_WRITE_RAM			0x5C
#define CMD_READ_RAM			0x5D
#define CMD_LOCK				0xFD

int reset(void)
{
    unsigned short wdata ;

    wdata = RST_BIT_MASK;
    write(fd,&wdata , 2 );
    usleep(2000);
    wdata = DEFAULT_MASK;
    write(fd,&wdata , 2 );
    return 1;
}

int oled_writeCmd(int size , unsigned short* cmdArr)
{
    int i ;
    unsigned short wdata;

    wdata = CS_BIT_MASK & DC_BIT_MASK;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK ;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK & (cmdArr[0]|0xFF00) ;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & (cmdArr[0] | 0xFF00) ;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & ( cmdArr[0] | 0xFF00);
    write(fd,&wdata,2);

    for (i = 1; i < size ; i++ )
    {
        wdata = CS_BIT_MASK & WD_BIT_MASK ;
        write(fd,&wdata,2);

        wdata = CS_BIT_MASK & WD_BIT_MASK & (cmdArr[i] | 0xFF00) ;
        write(fd,&wdata,2);

        wdata = CS_BIT_MASK & (cmdArr[i] | 0xFF00);
        write(fd,&wdata,2);
    }
    wdata= DEFAULT_MASK;
    write(fd,&wdata,2);
    return 1;
}

int writeData(int size , unsigned char* dataArr)
{
    int i ;
    unsigned short wdata;

    wdata = CS_BIT_MASK & DC_BIT_MASK;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK & (CMD_WRITE_RAM | 0xFF00) ;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & (CMD_WRITE_RAM | 0xFF00);
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK &  (CMD_WRITE_RAM | 0xFF00);
    write(fd,&wdata,2);

    for (i = 0; i < size ; i++ )
    {
        wdata = CS_BIT_MASK & WD_BIT_MASK ;
        write(fd,&wdata,2);

        wdata = CS_BIT_MASK & WD_BIT_MASK & ((unsigned char)dataArr[i] | 0xFF00 );
        write(fd,&wdata,2);

        wdata = CS_BIT_MASK & ( (unsigned char)dataArr[i] | 0xFF00);
        write(fd,&wdata,2);
    }
    wdata = DEFAULT_MASK;
    write(fd,&wdata,2);

    return 1;

}

int readData(int size , unsigned short* dataArr)
{

    int i ;
    unsigned short wdata;

    wdata = CS_BIT_MASK & DC_BIT_MASK;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & ( CMD_READ_RAM| 0xFF00) ;
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK &( CMD_READ_RAM| 0xFF00);
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK & DC_BIT_MASK & (CMD_READ_RAM | 0xFF00);
    write(fd,&wdata,2);

    wdata = CS_BIT_MASK &  (CMD_READ_RAM | 0xFF00);
    write(fd,&wdata,2);


    for (i = 0; i < size ; i++ )
    {
        wdata = CS_BIT_MASK ;
        write(fd,&wdata,2);

        wdata = CS_BIT_MASK & RD_BIT_MASK ;
        write(fd,&wdata,2);

        wdata = CS_BIT_MASK & RD_BIT_MASK ;
        write(fd,&wdata,2);

        wdata = CS_BIT_MASK ;
        write(fd,&wdata,2);

        read(fd,&dataArr[i],2);

    }
    wdata = DEFAULT_MASK;
    write(fd,&wdata ,2);

    return 1;
}

int setAddressDefalut(void)
{
    unsigned short  cmd[3];
    cmd[0] = CMD_SET_COLUMN_ADDR;
    cmd[1] = 0;
    cmd[2] = 127;
    oled_writeCmd(3,cmd);

    cmd[0] = CMD_SET_ROW_ADDR;
    cmd[1] = 0;
    cmd[2] = 127;
    oled_writeCmd(3,cmd);

    return 1;
}

// to send cmd  , must unlock
int setCmdLock(int bLock)
{
    unsigned short  cmd[3];

    cmd[0] = CMD_LOCK;
    if (bLock)
    {
        cmd[1] = 0x16; // lock
        oled_writeCmd(2,cmd);

    }
    else
    {
        cmd[1] = 0x12; // lock
        oled_writeCmd(2,cmd);

        // A2,B1,B3,BB,BE accessible
        cmd[1] = 0xB1;
        oled_writeCmd(2,cmd);
    }
    return 1;
}

int imageLoading(const char* fileName)
{
    int imgfile;
    unsigned char* data;
    int  width , height;

    imgfile = open(fileName , O_RDONLY );
    if ( imgfile < 0 )
    {
        printf ("imageloading(%s)  file is not exist . err.\n",fileName);
        return 0;
    }
    setCmdLock(0);


    read(imgfile ,&width , sizeof(unsigned char));
    read(imgfile ,&height , sizeof(unsigned char));

    data = (unsigned char*)malloc( 128 * 128 * 3 );

    read(imgfile, data , 128 * 128 *3 );

    close(imgfile);

    writeData(128 * 128 *3 , data );

    setCmdLock(1);
    return 1;
}

static unsigned short gamma[64]=
        {
                0xB8,
                0x02, 0x03, 0x04, 0x05,
                0x06, 0x07, 0x08, 0x09,
                0x0A, 0x0B, 0x0C, 0x0D,
                0x0E, 0x0F, 0x10, 0x11,
                0x12, 0x13, 0x15, 0x17,
                0x19, 0x1B, 0x1D, 0x1F,
                0x21, 0x23, 0x25, 0x27,
                0x2A, 0x2D, 0x30, 0x33,
                0x36, 0x39, 0x3C, 0x3F,
                0x42, 0x45, 0x48, 0x4C,
                0x50, 0x54, 0x58, 0x5C,
                0x60, 0x64, 0x68, 0x6C,
                0x70, 0x74, 0x78, 0x7D,
                0x82, 0x87, 0x8C, 0x91,
                0x96, 0x9B, 0xA0, 0xA5,
                0xAA, 0xAF, 0xB4

        };


int Init(void)
{
    unsigned short wdata[10];
    unsigned char  wcdata[10];
    int i,j;
    wdata[0]= 0xFD;
    wdata[1] = 0x12;
    oled_writeCmd(2,wdata);


    wdata[0] = 0xFD;
    wdata[1] = 0xB1;
    oled_writeCmd(2,wdata);

    wdata[0] = 0xAE;
    oled_writeCmd(1,wdata);

    wdata[0] = 0xB3;
    wdata[1] = 0xF1;
    oled_writeCmd(2,wdata);

    wdata[0] = 0xCA;
    wdata[1] = 0x7F;
    oled_writeCmd(2,wdata);

    wdata[0] = 0xA2;
    wdata[1] = 0x00;
    oled_writeCmd(2,wdata);

    wdata[0]= 0xA1;
    wdata[1]=0x00;
    oled_writeCmd(2,wdata);

    wdata[0]= 0xA0;
    wdata[1] = 0xB4;
    oled_writeCmd(2,wdata);

    wdata[0] = 0xAB;
    wdata[1] = 0x01;
    oled_writeCmd(2,wdata);

    wdata[0] = 0xB4;
    wdata[1] = 0xA0;
    wdata[2] = 0xB5;
    wdata[3] = 0x55;
    oled_writeCmd(4,wdata);

    wdata[0] = 0xC1;
    wdata[1] = 0xC8;
    wdata[2] = 0x80;
    wdata[3] = 0xC8;
    oled_writeCmd(4,wdata);

    wdata[0] = 0xC7;
    wdata[1] = 0x0F;
    oled_writeCmd(2,wdata);

    // gamma setting
    oled_writeCmd(64,gamma);


    wdata[0] = 0xB1;
    wdata[1] = 0x32;
    oled_writeCmd(2,wdata);

    wdata[0] = 0xB2;
    wdata[1] = 0xA4;
    wdata[2] = 0x00;
    wdata[3] = 0x00;
    oled_writeCmd(4,wdata);

    wdata[0] = 0xBB;
    wdata[1] = 0x17;
    oled_writeCmd(2,wdata);

    wdata[0] = 0xB6;
    wdata[1] = 0x01;
    oled_writeCmd(2, wdata);

    wdata[0]= 0xBE;
    wdata[1] = 0x05;
    oled_writeCmd(2, wdata);

    wdata[0] = 0xA6;
    oled_writeCmd(1,wdata);


    for (i = 0; i < 128;i++ )
    {
        for(j = 0; j < 128; j++ )
        {
            wcdata[0]= 0x3F;
            wcdata[1]= 0;
            wcdata[2] = 0;
            writeData(3,wcdata);
        }

    }

    wdata[0] = 0xAF;
    oled_writeCmd(1,wdata);

    return 1;
}

#define MODE_WRITE		0
#define MODE_READ		1
#define MODE_CMD		2
#define MODE_RESET		3
#define MODE_IMAGE		4
#define MODE_INIT		5

/*
static int Mode;
*/
void oledDisp(string str)
{
    pthread_mutex_lock(thread_manager::get_oled());
    pthread_mutex_lock(thread_manager::get_cled());
    pthread_mutex_lock(thread_manager::get_mled());
    pthread_mutex_lock(thread_manager::get_bled());
    pthread_mutex_lock(thread_manager::get_seg());
    pthread_mutex_lock(thread_manager::get_tlcd());
    pthread_mutex_lock(thread_manager::get_dips());
    pthread_mutex_lock(thread_manager::get_buzz());
    pthread_mutex_lock(thread_manager::get_key());
    
    reset();

    Init();

    imageLoading(str.c_str());

    pthread_mutex_unlock(thread_manager::get_oled());
    pthread_mutex_unlock(thread_manager::get_cled());
    pthread_mutex_unlock(thread_manager::get_mled());
    pthread_mutex_unlock(thread_manager::get_bled());
    pthread_mutex_unlock(thread_manager::get_seg());
    pthread_mutex_unlock(thread_manager::get_tlcd());
    pthread_mutex_unlock(thread_manager::get_dips());
    pthread_mutex_unlock(thread_manager::get_buzz());
    pthread_mutex_unlock(thread_manager::get_key());
    
    
    return;
}

void _oled(Shared* shared)
{
    // open  driver
    fd = open(DRIVER_OLED, O_RDWR);
    if ( fd < 0 )
    {
        perror("driver open error.\n");
        return;
    }
    Data* s = &(shared->sensor);
    Data* d = &(shared->data);
    bool chk[4];
    
    string prev = "", cur;

    while(1) {
        int cnt = 0;
        chk[0] = s->humidity >= d->humidity;
        chk[1] = s->temperature >= d->temperature;
        chk[2] = s->illumination >= d->illumination;
        chk[3] = s->soil_humidity <= d->soil_humidity;
        for(int i = 0 ; i < 4 ; i++)
        {
            if(chk[i])
            {
                cnt++;
            }
        }
        if(s->illumination <= 10)
        {
            cur = "oled_face_sleep.img";
        }
        else if(s->soil_humidity >= d->soil_humidity*2)
        {
            cur = "oled_face_confuse.img";
        }
        else if(cnt == 4)
        {
            cur = "oled_face_smile.img";
        }
        else if(cnt == 0)
        {
            cur = "oled_face_angry.img";
        }
        else
        {
            cur = "oled_face_normal.img";
        }

        if(cur != prev)
        {
            oledDisp(cur);
        }
        prev = cur;
        usleep(1000000);
    }
    close(fd);

    return;
}


void* oled(void* shared) {
    _oled((Shared*)shared);
}

