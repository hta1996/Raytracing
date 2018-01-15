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
    Color getCol(int i,int j)const{if(i<0)i=0;if(j<0)j=0;return Color(a[i][j].r,a[i][j].g,a[i][j].b)/255.;}
    Color getCol(double i,double j)const
    {
        i*=W-1;j*=H-1;
        //return getCol(int(i+0.5),int(j+0.5));
        int u1=floor(i+Const::eps),v1=floor(j+Const::eps),u2=u1+1,v2=v1+1;
        double ru=u2-i,rv=v2-j;
        if(u1<0)u1=W-1;if(v1<0)v1=H-1;
        if(u2==W)u2=0;if(v2==H)v2=0;
        return getCol(u1,v1)*(ru*rv)+getCol(u1,v2)*(ru*(1-rv))+getCol(u2,v1)*((1-ru)*rv)+getCol(u2,v2)*((1-ru)*(1-rv));
    }

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
