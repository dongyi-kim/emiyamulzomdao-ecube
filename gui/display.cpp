//
// Created by parallels on 6/12/16.
//

#include "display.h"

#include <stdio.h>
#include <stdlib.h>     // for exit
#include <unistd.h>     // for open/close
#include <fcntl.h>      // for O_RDWR
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include "bitmap.h"

#define FBDEV_FILE  "/dev/fb0"
#define BIT_VALUE_24BIT   24

#include <vector>

using namespace std;

int screen_width;
int screen_height;
int bits_per_pixel;
int line_length;
unsigned char   *pfbmap;
typedef unsigned long pixel_t;

namespace display{
    int init(){
        int fbfd;

        int coor_x, coor_y;
        int cols = 0, rows = 0;
        int mem_size;

        char    *pData, *data;
        char    r, g, b;
        unsigned long   bmpdata[1280*800];
        unsigned long   pixel;

        unsigned long   *ptr;
        struct  fb_var_screeninfo fbvar;
        struct  fb_fix_screeninfo fbfix;

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

        munmap( pfbmap, mem_size);
        close( fbfd);
    }



    pixel_t& lcd(int x, int y){
        int lcd_y = x;
        int lcd_x = screen_width - y;
        return *((unsigned long*)pfbmap + (screen_width * lcd_y) + lcd_x);
    }

    void draw_bmp(const vector<vector<pixel_t> > &bitmap, int x = 0, int y = 0)
    {
        for(int r = 0 ; r < bitmap.size(); r++){
            for(int c = 0; c < bitmap[0].size(); c++){
                lcd(x, y) = bitmap[r][c];
            }
        }
    }

    void read_bmp(vector<vector<unsigned long> > &bitmap, const string& filename)
    {
        BITMAPFILEHEADER    bmpHeader;
        BITMAPINFOHEADER    *bmpInfoHeader;
        unsigned int    size;
        unsigned char   magicNum[2];
        int     nread;
        FILE    *fp = fopen(filename.c_str(), "rb");
        if(fp == NULL) {
            //printf("ERROR\n");
            return;
        }

        // identify bmp file
        magicNum[0]   =   fgetc(fp);
        magicNum[1]   =   fgetc(fp);
        //printf("magicNum : %c%c\n", magicNum[0], magicNum[1]);

        if(magicNum[0] != 'B' && magicNum[1] != 'M') {
            //printf("It's not a bmp file!\n");
            fclose(fp);
            return;
        }

        nread   =   fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
        size    =   bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
        *pDib   =   (unsigned char *)malloc(size);      // DIB Header(Image Header)
        fread(*pDib, 1, size, fp);
        bmpInfoHeader   =   (BITMAPINFOHEADER *)*pDib;

        //printf("nread : %d\n", nread);
        //printf("size : %d\n", size);

        // check 24bit
        if(BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount))     // bit value
        {
            //printf("It supports only 24bit bmp!\n");
            fclose(fp);
            return;
        }

        *cols   =   bmpInfoHeader->biWidth;
        *rows   =   bmpInfoHeader->biHeight;

        bitmap.clear();
        bitmap.resize(rows);
        for(int j = 0; j < rows; j++)
        {
            bitmap[j].resize(cols);
            k   =   j * cols * 3;
            t   =   (rows - 1 - j) * cols;

            for(int i = 0; i < cols; i++)
            {
                b   =   *(data + (k + i * 3));
                g   =   *(data + (k + i * 3 + 1));
                r   =   *(data + (k + i * 3 + 2));

                pixel = ((r<<16) | (g<<8) | b);
                bitmap[r][c] = pixel;
            }
        }
        fclose(fp);
    }

}


