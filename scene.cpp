//
//  scene.cpp
//  Raytracing
//
//  Created by hta on 2017/12/3.
//  Copyright © 2017年 hta. All rights reserved.
//
/*
#include <stdio.h>
#include "scene.h"
#include<string>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<ctime>

void Camera::input(std::string arg,std::stringstream &fin)
{
    if(arg=="aperture:")fin>>aperture;
    else if(arg=="eye:")O.input(fin);
    else if(arg=="lookat:"){Look.input(fin);D=(Look-O).normalize();focalLen=D.len();}
    else if(arg=="up:")Up.input(fin);
    else if(arg=="w:"){fin>>W;init();}
    else if(arg=="h:")fin>>H;
    else if(arg=="fovy:"){fin>>fovy;fovy*=Const::pi/180;}
    else if(arg=="focal:")fin>>focalLen;
}
void Camera::init()
{
    Dw=(D*Up).normalize()*tan(fovy/2)*(1.*W/H);
    Dh=(D*Dw).normalize()*tan(fovy/2);
    a=new Color*[W];
    for(int i=0;i<W;i++)a[i]=new Color[H];
}
Camera::~Camera()
{
    for(int i=0;i<W;i++)delete[] a[i];
    delete[] a;
}
void Scene::inputAmbientCol(std::string arg,std::stringstream &fin)
{
    if(arg=="ambientcolor:")ambientCol.input(fin);
}
void Scene::addPrimitive(Primitive *pri)
{
    if(pri)pri->setNext(priHead),priHead=pri;
}
void Scene::addLight(Light *light)
{
    if(light)light->setNext(lightHead),lightHead=light;
}*/

