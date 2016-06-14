/**    
    @file       display.cpp
    @author     dongyi kim
    @brief      about 
*/

#include "display.h"

#include <stdio.h>
#include <stdlib.h>     // for exit
#include <unistd.h>     // for open/close
#include <fcntl.h>      // for O_RDWR
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO

#define FBDEV_FILE  "/dev/fb0"
#define BIT_VALUE_24BIT   24

#include <vector>
#include <string>
#include <pthread.h>
#include <iostream>
using namespace std;



namespace display
{

    int screen_width    =   1280;
    int screen_height   =   800;
    int bits_per_pixel;
    int line_length;
    unsigned char   *pfbmap = NULL;
    typedef unsigned long pixel_t;
    unsigned long   bmpdata[1280*800];

    pthread_mutex_t display_mutex;

    /**
        @return    result code of initializeing 
        @warning King\kong\file.dat와 같은 파일명은 포함 안된다.
    */
    int init()
    {
        //init mutex
        pthread_mutex_init(&display_mutex, NULL);
        
        //fiil all screen with black-pixel
        vector<vector<unsigned long> > vscreen(screen_width, vector<unsigned long>(screen_height, 0xFFFFFF));
        draw_bmp(vscreen);
        return 0;
    }

    /**
        @params     coordinate based on left-top axis 
        @return     pointer of LCD Panel 
    */
    unsigned long* lcd(int x, int y)
    {   
        if(pfbmap == NULL)
        {   //Exception. Display not ready.
            return NULL;
        }
        //calculate coordinate based on left bottop axis 
        int lcd_y = x;
        int lcd_x = screen_width - y - 1;
        
        //if coordinate on the outside of the display, return NULL
        if(lcd_y >= screen_height || lcd_x >= screen_width || lcd_x < 0 || lcd_y < 0)
        {
            return NULL;
        }
        
        //return pointer 
        return ((unsigned long*)pfbmap + (screen_width * lcd_y) + lcd_x);
    }

    /**
        @params 
    */
    void draw_bmp(const vector<vector<unsigned long> > &bitmap, int x, int y )
    {

        int fbfd;
        int coor_x, coor_y;
        int cols = 0, rows = 0;
        int mem_size;

        char    *pData, *data;
        char    r, g, b;
        unsigned long   pixel;

        unsigned long   *ptr;
        struct  fb_var_screeninfo fbvar;
        struct  fb_fix_screeninfo fbfix;

        //prevent race conditions on the display
        pthread_mutex_lock(&display_mutex);
        
        printf("[display_mutex] lock\n");

        if( (fbfd = open(FBDEV_FILE, O_RDWR)) < 0)
        {
            printf("%s: open error\n", FBDEV_FILE);
            exit(1);
        }

        if( ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar) )
        {
            printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
            exit(1);
        }

        if( ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix) )
        {
            printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
            exit(1);
        }

        if (fbvar.bits_per_pixel != 32)
        {
            fprintf(stderr, "bpp is not 32\n");
            exit(1);
        }

        //get device information
        screen_width    =   fbvar.xres;
        screen_height   =   fbvar.yres;
        bits_per_pixel  =   fbvar.bits_per_pixel;
        line_length     =   fbfix.line_length;
        mem_size    =   line_length * screen_height;
        pfbmap  =   (unsigned char *) mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

        if ((unsigned)pfbmap == (unsigned)-1)
        {
            perror("fbdev mmap\n");
            exit(1);
        }

        pfbmap  =   (unsigned char *) mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

        //draw image from (x, y) ~ ( x + cols - 1, y + rows - 1);
        for(int r = 0 ; r < bitmap.size(); r++)
        {
            for(int c = 0; c < bitmap[0].size(); c++)
            {
                //get the pointer of display panel
                unsigned long* ptr = lcd(x+c,y+r);
                
                //if null ptr, don't draw on this coordinate
                if(ptr == NULL)
                {   
                    continue;
                }
                
                //draw
                *ptr = bitmap[r][c];
            }
        }

        //save display
        munmap( pfbmap, mem_size);
        close( fbfd);
        printf("[display_mutex] unlock\n");
        
        //unlock mutex
        pthread_mutex_unlock(&display_mutex);
    }

    void read_bmp(vector<vector<unsigned long> > &bitmap, const string& filename)
    {
        BITMAPFILEHEADER    bmpHeader;
        BITMAPINFOHEADER    *bmpInfoHeader;
        unsigned int    size;
        unsigned char   magicNum[2];
        int     nread;
        
        printf("[display] file read : %s\n", filename.c_str());
        FILE    *fp = fopen(filename.c_str(), "rb");
        if(fp == NULL) {
            printf("ERROR\n");
            return;
        }
        
        magicNum[0]   =   fgetc(fp);
        magicNum[1]   =   fgetc(fp);
        
        if(magicNum[0] != 'B' && magicNum[1] != 'M') {
            //printf("It's not a bmp file!\n");
            fclose(fp);
            return;
        }
        nread   =   fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
        size    =   bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
        
        unsigned char* pDib   =   (unsigned char *)malloc(size);      // DIB Header(Image Header)
        fread(pDib, 1, size, fp);
        bmpInfoHeader   =   (BITMAPINFOHEADER *)pDib;

        // check 24bit
        if(BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount))     // bit value
        {
            printf("It supports only 24bit bmp!\n");
            fclose(fp);
            return;
        }

        int cols   =   bmpInfoHeader->biWidth;
        int rows   =   bmpInfoHeader->biHeight;

        char* data =   (char *)(pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);

        bitmap.clear();
        bitmap.resize(rows);
        
        for(int j = 0; j < rows; j++)
        {
            bitmap[rows-j-1].resize(cols);
            int k   =   j * cols * 3;
            int t   =   (rows - 1 - j) * cols;

            for(int i = 0; i < cols; i++)
            {
                char b   =   *(data + (k + i * 3));
                char g   =   *(data + (k + i * 3 + 1));
                char r   =   *(data + (k + i * 3 + 2));
                
                //cover to BGR data to RGB
                //save into bitmap vector
                bitmap[rows-j-1][i] = ((r<<16) | (g<<8) | b);
            }
        }
        fclose(fp);
    }

}
