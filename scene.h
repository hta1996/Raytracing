//
//  scene.h
//  Raytracing
//
//  Created by hta on 2017/12/3.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef scene_h
#define scene_h
#include "object.h"
#include <stdio.h>
#include<string>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<ctime>
/*
class Camera
{
public:
    ~Camera();
    void input(std::string arg,std::stringstream &fin);
    void setCol(int i,int j,const Color &col){a[i][j]=col;}
    Color getCol(int i,int j)const{return(0<=i&&i<W&&0<=j&&j<H)?a[i][j]:Color();}
    Ray emit(double x,double y)const{return Ray(O,D+Dw*(2.*x/W-1)+Dh*(2.*y/H-1));}///单位化了方向！！
    void print(const std::string &file)
    {
        Bmp* pic=new Bmp(W,H);
        for(int i=0;i<W;i++)
            for(int j=0;j<H;j++)pic->setCol(i,j,a[i][j].confine());
        pic->output(file);
    }
private:
    Vec O,Look,D,Up; // 相机位置、相机视线上任意一点、视线方向、上方向
    int W,H;                          // 分辨率
    double fovy;                         // 相机视野张角
    double aperture,focalLen;        // 光圈大小，焦距(默认为 O 和 Look 的距离)
    Vec Dw,Dh;                    // 视平面坐标系方向
    Color **a;// 颜色缓存
    //输入文件中，eye，look，focal_len的顺序读入,W最后
    void init();
};

class Scene
{
public:
    void inputAmbientCol(std::string arg,std::stringstream &fin);
    void addPrimitive(Primitive *pri);
    void addLight(Light *light);
    void createScene(std::string file)
    {
        srand(19961015);
        std::ifstream fin(file.c_str());
        std::string obj;
        while(fin>>obj)
        {
            Primitive* new_pri=nullptr;
            Light* new_light=nullptr;
            if(obj=="primitive")
            {
                std::string type;
                fin>>type;
                if(type=="sphere")new_pri=new Sphere;
                if(type=="plane")new_pri=new Plane;
                //if ( type == "rectangle" ) new_primitive = new Rectangle;
                //if ( type == "triangle" ) new_primitive = new Triangle;
                //if ( type == "polyhedron" ) new_primitive = new Polyhedron;
                addPrimitive(new_pri);
            }else if(obj=="light")
            {
                std::string type;fin>>type;
                if(type=="point")new_light=new Pointlight;
                if(type=="area")new_light=new Arealight;
                addLight(new_light);
            }else if(obj!="background"&&obj!="camera")continue;
            fin.ignore( 1024 , '\n' );
            std::string order;
            while(getline(fin,order,'\n'))
            {
                std::stringstream fin2(order);
                std::string var;fin2>>var;
                if (var=="end")break;
                if(obj=="background")inputAmbientCol(var,fin2);
                if(obj=="primitive"&&new_pri!=NULL)new_pri->input(var,fin2);
                if(obj=="light"&&new_light!= NULL)new_light->input(var,fin2);
                if(obj=="camera")camera->input(var,fin2);
            }
        }
    }
    Collider findCollision(const Ray &ray)const;
private:
    Camera *camera;
    Color ambientCol;
    Primitive *priHead;
    Light *lightHead;
};
*/

#endif /* scene_h */
