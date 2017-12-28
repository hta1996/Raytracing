//
//  bmp.cpp
//  Raytracing
//
//  Created by hta on 2017/11/30.
//  Copyright © 2017年 hta. All rights reserved.
//

#include"bmp.h"
#include<cstdio>
#include<fstream>
#include<iostream>
#include<string>
#include<cmath>
using namespace std;

Bmp::Bmp(int w,int h)
{
    strHead.bfReserved1 = 0;
    strHead.bfReserved2 = 0;
    strHead.bfOffBits = 54;
    
    strInfo.biSize = 40;
    strInfo.biPlanes = 1;
    strInfo.biHeight = h;
    strInfo.biWidth = w;
    strInfo.biBitCount = 24;
    strInfo.biCompression = 0;
    strInfo.biSizeImage =0;// H * W * 3;
    strInfo.biXPelsPerMeter = 0;
    strInfo.biYPelsPerMeter = 0;
    strInfo.biClrUsed = 0;
    strInfo.biClrImportant = 0;
    
    strHead.bfSize = H * W * 3 + strInfo.biBitCount;
    H=h;W=w;
    a=new Bcol*[w];
    for(int i=0;i<w;i++)a[i]=new Bcol[h];
}
Bmp::~Bmp()
{
    for(int i=0;i<W;i++)delete[] a[i];
    delete[] a;
}
Bmp::Bmp(std::string file)
{
    FILE* f=fopen(file.c_str(),"rb");
    if(!f)
    {
        cerr<<"No Bmp file "<<file<<endl;
        return;
    }
    cerr<<"read!"<<endl;

    word bfType;
    fread(&bfType,1,sizeof(word),f);
    fread(&strHead,1,sizeof(BITMAPFILEHEADER),f);
    fread(&strInfo,1,sizeof(BITMAPINFOHEADER),f);
    H=strInfo.biHeight;
    W=strInfo.biWidth;
    cerr<<H<<" "<<W<<endl;
    cerr<<(int)strInfo.biClrUsed<<endl;
    byte pltcol;
    for(int i=0;i<(int)strInfo.biClrUsed;i++)
        for(int j=0;j<3;j++)
            fread((char*)&pltcol,1,sizeof(byte),f);
    a=new Bcol*[W];
    for(int i=0;i<W;i++)a[i]=new Bcol[H];
    for(int j=H-1;j>=0;j--)
        for(int i=0;i<W;i++)
        {
            fread(&a[i][j].b,1,sizeof(byte),f);
            fread(&a[i][j].g,1,sizeof(byte),f);
            fread(&a[i][j].r,1,sizeof(byte),f);
        }
    cerr<<"read!"<<endl;
    fclose(f);
}
void Bmp::output(std::string file)
{
    FILE* f=fopen(file.c_str(),"wb");
    cerr<<H<<" "<<W<<endl;

    word bfType=0x4d42;
    fwrite(&bfType,1,sizeof(word),f);
    fwrite(&strHead,1,sizeof(BITMAPFILEHEADER),f);
    fwrite(&strInfo,1,sizeof(BITMAPINFOHEADER),f);
    //for(int i=0;i<H;i++)
      //  for(int j=0;j<W;j++)
    for(int j=H-1;j>=0;j--)
        for(int i=0;i<W;i++)
        {
            fwrite(&a[i][j].b,1,sizeof(byte),f);
            fwrite(&a[i][j].g,1,sizeof(byte),f);
            fwrite(&a[i][j].r,1,sizeof(byte),f);
        }
    fclose(f);
}

