//
//  bmp.h
//  Raytracing
//
//  Created by hta on 2017/11/30.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef bmp_h
#define bmp_h
#include "common.h"
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
struct BITMAPFILEHEADER
{
    //word bfType;
    dword bfSize;
    word bfReserved1;
    word bfReserved2;
    dword bfOffBits;
};
struct BITMAPINFOHEADER
{
    dword biSize;
    dword biWidth;
    dword biHeight;
    word biPlanes;
    word biBitCount;
    dword biCompression;
    dword biSizeImage;
    dword biXPelsPerMeter;
    dword biYPelsPerMeter;
    dword biClrUsed;
    dword biClrImportant;
};
class Bmp
{
    BITMAPFILEHEADER strHead;
    BITMAPINFOHEADER strInfo;
    struct Bcol
    {
        byte r,g,b;
    };
    Bcol **a;
    int H,W;
public:
    Bmp(int h=0,int w=0);
    Bmp(std::string file);
    ~Bmp();
    int geth(){return H;}
    int getw(){return W;}
    Color getCol(int i,int j)const{return Color(a[i][j].r,a[i][j].g,a[i][j].b)/255.;}
    Color getCol(double i,double j)const{i*=W-1;j*=H-1;return getCol(int(i+0.5),int(j+0.5));}

    void setCol(int i,int j,const Color &col)
    {
        a[i][j].r=int(col.r*255);
        a[i][j].g=int(col.g*255);
        a[i][j].b=int(col.b*255);
    }
    void init(int h,int w);
    void output(std::string file);
    
};

#endif /* bmp_h */
