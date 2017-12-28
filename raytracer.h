//
//  raytracer.h
//  Raytracing
//
//  Created by hta on 2017/11/20.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef raytracer_h
#define raytracer_h
#include "object.h"
#include<cstdio>
#include<fstream>
#include<iostream>
#include<string>
#include<cmath>
using namespace std;

class Raytracer
{
public:
    //Raytracer(Scene *s){scene=s;}
    Raytracer(std::string file)
    {
        scene=new Scene;
        scene->createScene(file);
        cerr<<"scene built!"<<endl;
        camera=scene->getCamera();
        H=camera->getH();
        W=camera->getW();
        hash=new int*[W];
        vis=new bool*[W];
        for(int i=0;i<W;i++)
        {
            hash[i]=new int[H],vis[i]=new bool[H];
            memset(vis[i],0,H);
        }
    }
    virtual void Run(const std::string &file);
    Color calcCol(double x,double y,int *hash,double fac=1);
    Color calcECol(int x,int y,int *hash);
    Color rayTracing(const Ray &ray,const Color &fac,double weight,int depth,int *hash);
    void antiAliasing();
protected:
    Scene *scene;
    Camera *camera;
    int H,W,curx,cury;
    int **hash;
    bool **vis,flag;
    virtual Color calcIllumination(const Collider &tp,const Material* material,const Color &fac)const;
};

#endif /* raytracer_h */
