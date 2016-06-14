/**
    @file       display.h
    @author     dongyi kim
    @brief      about touch event
*/
#ifndef EMIYAMULZOMDAO_ECUBE_DISPLAY_H
#define EMIYAMULZOMDAO_ECUBE_DISPLAY_H
#include<vector>
#include<string>
using namespace std;
// BMP File Structure (windows version 3)

// File Header
typedef struct {
//    unsigned char   bfType;         // 2 byte
    unsigned int    bfSize;         // 4 byte
    unsigned short  bfReserved1;    // 2 byte
    unsigned short  bfReserved2;    // 2 byte
    unsigned int    bfOffBits;      // 4 byte
} BITMAPFILEHEADER;

// Image Header
typedef struct {
    unsigned int    biSize;             // 4 byte
    unsigned int    biWidth;            // 4 byte
    unsigned int    biHeight;           // 4 byte
    unsigned short  biPlanes;           // 2 byte
    unsigned short  biBitCount;         // 2 byte
    unsigned int    biCompression;      // 4 byte
    unsigned int    biSizeImage;        // 4 byte
    unsigned int    biXPelsPerMeter;    // 4 byte
    unsigned int    biYPelsPerMeter;    // 4 byte
    unsigned int    biClrUsed;          // 4 byte
    unsigned int    biClrImportant;     // 4 byte
} BITMAPINFOHEADER;

// Color Table
typedef struct {
// windows version 3
    unsigned char   rgbBlue;        // 1 byte
    unsigned char   rgbGreen;       // 1 byte
    unsigned char   rgbRed;         // 1 byte
    unsigned char   rgbReserved;    // 1 byte
} RGBQUAD;

// Pixel Data
typedef struct {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;

namespace display{
    int init();

    void draw_bmp(const vector<vector<unsigned long> > &bitmap, int x=0 , int y=0 );
    void read_bmp(vector<vector<unsigned long> > &bitmap, const string& filename);

}
#endif //EMIYAMULZOMDAO_ECUBE_DISPLAY_H
