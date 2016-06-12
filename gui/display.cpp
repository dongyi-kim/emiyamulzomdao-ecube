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

#define FBDEV_FILE  "/dev/fb0"
#define BIT_VALUE_24BIT   24

#include <vector>
#include <string>
#include <pthread>
#include <iostream>
using namespace std;



namespace display{

    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    unsigned char   *pfbmap;
    typedef unsigned long pixel_t;
    unsigned long   bmpdata[1280*800];

    pthread_mutex_t display_mutex;

    int init(){
        pthread_mutex_init(&display_mutex, NULL);


    }


    unsigned long* lcd(int x, int y){
        int lcd_y = x;
        int lcd_x = screen_width - y - 1;
        //printf("(%d, %d) -> (%d, %d) \n",x ,y, lcd_x, lcd_y);
        return ((unsigned long*)pfbmap + (screen_width * lcd_y) + lcd_x);
    }

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
        pthread_mutex_lock(&display_mutex);

        int ecnt = 0;
        if( (fbfd = open(FBDEV_FILE, O_RDWR)) < 0)
        {
            printf("%s: open error\n", FBDEV_FILE);
            exit(1);
        }

        cout << ++ecnt << endl;
        if( ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar) )
        {
            printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
            exit(1);
        }

        cout << ++ecnt << endl;
        if( ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix) )
        {
            printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
            exit(1);
        }

        cout << ++ecnt << endl;
        if (fbvar.bits_per_pixel != 32)
        {
            fprintf(stderr, "bpp is not 32\n");
            exit(1);
        }

        cout << ++ecnt << endl;
        screen_width    =   fbvar.xres;
        screen_height   =   fbvar.yres;
        bits_per_pixel  =   fbvar.bits_per_pixel;
        line_length     =   fbfix.line_length;
        mem_size    =   line_length * screen_height;
        pfbmap  =   (unsigned char *) mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

        cout << ++ecnt << endl;
        if ((unsigned)pfbmap == (unsigned)-1)
        {
            perror("fbdev mmap\n");
            exit(1);
        }

        cout << ++ecnt << endl;
        pfbmap  =   (unsigned char *) mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
        for(int r = 0 ; r < bitmap.size(); r++){
            for(int c = 0; c < bitmap[0].size(); c++){
                unsigned long* ptr = lcd(x+c,y+r);
                *ptr = bitmap[r][c];
            }
        }

        cout << ++ecnt << endl;
        munmap( pfbmap, mem_size);
        close( fbfd);
        pthread_mutex_lock(&display_mutex);
    }

    void read_bmp(vector<vector<unsigned long> > &bitmap, const string& filename)
    {
        printf("[file read] %s\n", filename.c_str());
        int ecnt = 0;

        BITMAPFILEHEADER    bmpHeader;
        BITMAPINFOHEADER    *bmpInfoHeader;
        unsigned int    size;
        unsigned char   magicNum[2];
        int     nread;
        FILE    *fp = fopen(filename.c_str(), "rb");
        if(fp == NULL) {
            printf("ERROR\n");
            return;
        }

//        cout << ++ecnt << endl;

        // identify bmp file
        magicNum[0]   =   fgetc(fp);
        magicNum[1]   =   fgetc(fp);
        //printf("magicNum : %c%c\n", magicNum[0], magicNum[1]);


//        cout << ++ecnt << endl;

        if(magicNum[0] != 'B' && magicNum[1] != 'M') {
            //printf("It's not a bmp file!\n");
            fclose(fp);
            return;
        }

//        cout << ++ecnt << endl;


        nread   =   fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);

//        cout << "read" << nread  << ":" <<sizeof(BITMAPFILEHEADER) << endl;
        size    =   bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
//        cout << "size:" << size << endl;
        unsigned char* pDib   =   (unsigned char *)malloc(size);      // DIB Header(Image Header)
        fread(pDib, 1, size, fp);
        bmpInfoHeader   =   (BITMAPINFOHEADER *)pDib;

//        cout << "bmp" << bmpInfoHeader->biBitCount << endl;

//        cout << ++ecnt << endl;

        //printf("nread : %d\n", nread);
        //printf("size : %d\n", size);

        // check 24bit
        if(BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount))     // bit value
        {
            printf("It supports only 24bit bmp!\n");
            fclose(fp);
            return;
        }



//        cout << "a" << endl;
        int cols   =   bmpInfoHeader->biWidth;
        int rows   =   bmpInfoHeader->biHeight;

//        cout << "b" << endl;
        char* data =   (char *)(pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);

//        cout << "c" << endl;
        bitmap.clear();
        bitmap.resize(rows);

//        cout << "d" << endl;
        for(int j = 0; j < rows; j++)
        {
            bitmap[j].resize(cols);
            int k   =   j * cols * 3;
            int t   =   (rows - 1 - j) * cols;

            for(int i = 0; i < cols; i++)
            {
                char b   =   *(data + (k + i * 3));
                char g   =   *(data + (k + i * 3 + 1));
                char r   =   *(data + (k + i * 3 + 2));

                bitmap[rows-j-1][i] = ((r<<16) | (g<<8) | b);
            }
        }
        fclose(fp);
    }

}


