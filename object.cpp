//
//  primitive.cpp
//  Raytracing
//
//  Created by hta on 2017/12/2.
//  Copyright © 2017年 hta. All rights reserved.
//

#include "object.h"

void Material::input(std::string arg,std::stringstream &fin)
{
    if(arg=="color:")color.input(fin);
    else if(arg=="absor:")absor.input(fin);
    else if(arg=="refl:")fin>>refl;
    else if(arg=="refr:")fin>>refr;
    else if(arg=="diff:")fin>>diff;
    else if(arg=="spec:")fin>>spec;
    if(arg=="rindex:")fin>>rindex;
    if(arg=="texture:")
    {
        std::string file;
        fin>>file;
        texture=new Bmp(file);
    }
}
Color Material::textureColor(double u, double v)const
{
    if(texture)return texture->getCol(u,v);
    else return Color(1,1,1);
}

void Primitive::input(std::string arg, std::stringstream &fin)
{
    material->input(arg,fin);
}

void Light::input(std::string arg, std::stringstream &fin)
{
    if(arg=="color:")color.input(fin);
    else if(arg=="power:")fin>>power;
}

/*Collider Arealight::collide(const Ray& ray)const
{
    Ray uray=Ray(ray.O,ray.D);
    double c=N.dot(uray.D);
    if(fabs(c)<Const::eps)return Collider();
    double tp=N.dot(O)-N.dot(uray.O),t=tp/c;
    if(t<Const::eps)return Collider();
    Vec p=uray.get(t)-O;
    if (fabs(p.dot(Dx))+Const::eps<Dx.len2()&&fabs(p.dot(Dy))+Const::eps<Dy.len2())
        return Collider(uray,t,this,this->getSample());
    else return Collider();
}*/
#include <stdio.h>
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
}
void Scene::createScene(std::string file)
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
            if(type=="cylinder")new_pri=new Cylinder;
            if(type=="sphere")new_pri=new Sphere;
            if(type=="plane")new_pri=new Plane;
            if(type=="rotationbody")new_pri=new RotationBody;
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
Collider Scene::findCollision(const Ray &ray)const
{
    Collider ret;
    for(Light *itr=lightHead;itr!=NULL;itr=itr->getNext())
    {
        Collider tp=itr->collide(ray);
        if(tp.hit()&&tp.dist+Const::eps<ret.dist)ret=tp;
    }
    for(Primitive *itr=priHead;itr!=NULL;itr=itr->getNext())
    {
        Collider tp=itr->collide(ray);
        if(tp.hit()&&tp.dist+Const::eps<ret.dist)ret=tp;
    }
    return ret;
}



