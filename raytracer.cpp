//
//  raytracer.cpp
//  Raytracing
//
//  Created by hta on 2017/11/20.
//  Copyright © 2017年 hta. All rights reserved.
//

#include "raytracer.h"
#include<cstdio>
#include<fstream>
#include<iostream>
#include<string>
#include<cmath>
#include <ctime>
#include <vector>
using namespace std;

void Raytracer::Run(const std::string &file)
{
    cerr<<"Ray tracing..."<<endl;
    clock_t lastRefreshTime=clock();
    for(int i=0;i<W;i++)
    {
        for(int j=0;j<H;j++)
        {
            if(!j&&i%10==0)cerr<<"Ray tracing: column " << i <<endl;
            //m_pixel_x = i, m_pixel_y = j;
            //m_hash[i][j] = 0;
            //Color c=Color(0.5,0.5,0.5);
            Color c=calcCol(i,j,&hash[i][j]);
            camera->setCol(i,j,c);
        }
        /*if (Const::output_refresh_interval>0&&clock()-lastRefreshTime>Const::output_refresh_interval * CLOCKS_PER_SEC)
        {
            lastRefreshTime=clock();
            camera->print(file.c_str());
        }*/
    }
    camera->print(("RT"+file).c_str());
    if(Const::antiAliasingT)
    {
        antiAliasing();
        camera->print(("Smooth"+file).c_str());
    }
}

void Raytracer::antiAliasing()
{
    cerr<<"Smoothing"<<endl;
    int dx[]={0,1,0,1};
    int dy[]={0,0,1,1};
    vector<pair<int,int>> a;
    a.clear();
    for(int i=0;i<W-1;i++)
        for(int j=0;j<H-1;j++)
        {
            //if(!j&&i%10==0)cerr<<"Smoothing: column " << i <<endl;
            //if ((i==0||hash[i][j]==hash[i-1][j])&&(i==H-1||hash[i][j]==hash[i+1][j])&&
                //(j==0||hash[i][j]==hash[i][j-1])&&(j==W-1||hash[i][j]==hash[i][j+1]))continue;
            if(hash[i][j]==hash[i+1][j+1]&&hash[i+1][j]==hash[i][j+1])continue;
            for(int k=0;k<4;k++)
            {
                int x=i+dx[k],y=j+dy[k];
                if(!vis[x][y])vis[x][y]=1,a.push_back(make_pair(x,y));
            }
        }
    cerr<<a.size()<<endl;
    int cntt=0;
    for(auto p:a)
    {
        if(++cntt%1000==0)cerr<<cntt<<endl;
        int X=p.first,Y=p.second;
        vector<pair<double, double>> b;
        int T=Const::antiAliasingT;
        for (int i=0;i<T*2;i++)
            for (int j=0;j<T*2;j++)
            {
                // 旋转网格采样
                double t=atan(0.5);
                double x=(i+0.5)/T-1,y=(j+0.5)/T-1;
                double dx=x*cos(t)-y*sin(t),dy=x*sin(t)+y*cos(t);
                if (fabs(dx)<0.5&&fabs(dy)<0.5)b.push_back(make_pair(X+dx,Y+dy));
            }
        Color c(0,0,0);int tp;
        for (auto q:b)
            c=c+calcCol(q.first,q.second,&tp,1.0/b.size());
        camera->setCol(X,Y,c);
    }
}

Color Raytracer::calcCol(double x,double y,int *hash,double fac)
{
    //return rayTracing(camera->emit(x,y),Color(1,1,1)*fac,1,1,hash);
    if(camera->getAperture()<Const::eps||Const::depth_of_field_samples<=1)
        return rayTracing(camera->emit(x,y),Color(1,1,1)*fac,1,1,hash);
    Color res(0,0,0);
    for(int i=0;i<Const::depth_of_field_samples;i++)
        res=res+rayTracing(camera->emitDOF(x,y),Color(1,1,1)*fac/Const::depth_of_field_samples,1,1,hash);
    return res;
}

Color Raytracer::calcECol(int X,int Y,int *hash)
{
    vector<pair<double, double>> b;
    int T=Const::antiAliasingT;
    for (int i=0;i<T*2;i++)
        for (int j=0;j<T*2;j++)
        {
            // 旋转网格采样
            double t=atan(0.5);
            double x=(i+0.5)/T-1,y=(j+0.5)/T-1;
            double dx=x*cos(t)-y*sin(t),dy=x*sin(t)+y*cos(t);
            if (fabs(dx)<0.5&&fabs(dy)<0.5)b.push_back(make_pair(X+dx,Y+dy));
        }
    Color c(0,0,0);int tp;
    for (auto q:b)
        c=c+calcCol(q.first,q.second,&tp,1.0/b.size());
    return c;
}



Color Raytracer::calcIllumination(const Collider &tp,const Material* material,const Color &fac)const
{
    Color col=material->color*tp.primitive->textureColor(tp);
    Color res=col*scene->getAmbientCol()*material->diff;// 环境光
    for (Light *itr=scene->startLight();itr!=NULL;itr=itr->getNext())
    {
        Vec l=(itr->getSource()-tp.P).normalize();
        if(l.dot(tp.N)<Const::eps)continue;
        //double shade=itr->getShadowRatio(m_scene, coll.p);
        double shade=scene->calcShade(itr,tp.P);
        if(shade>Const::eps)
            res=res+col*itr->getCol()*material->BRDF(l,tp.N,tp.ray.D)*shade;
    }
    return res*fac;
}


Color Raytracer::rayTracing(const Ray &ray,const Color &fac,double weight,int depth,int *hash)
{
    if(depth>Const::maxRaytracingDepth)return Color();
    if(weight<Const::raytracing_min_weight)return Color();
    Collider tp=scene->findCollision(ray);
    if (!tp.hit())
    {
        return scene->getAmbientCol()*fac;
    }
    if(tp.hitLight())
    {
        (*hash*=Const::hashseed)+=tp.light->getSample();
        return tp.light->getCol()*fac;
    }
    Color res,absor(1,1,1);
    const Primitive* pri=tp.primitive;
    const Material* material = pri->getMaterial();
    (*hash*=Const::hashseed)+=tp.primitive->getSample();
    if(tp.internal)
    {
        absor=(material->absor*(-tp.dist));
        absor=Color(exp(absor.r),exp(absor.g),exp(absor.b));
    }
    if (material->diff>Const::eps||material->spec>Const::eps)
        res=res+calcIllumination(tp,material,fac*absor);
    if (material->refl>Const::eps||material->refr>Const::eps)
    {
        double rindex=material->rindex;
        if(tp.internal)rindex=1/rindex;
        Vec Refl=tp.ray.D.reflect(tp.N);
        Vec Refr=tp.ray.D.refract(tp.N,rindex);
        if(material->refr<Const::eps) // 全镜面反射
            res=res+rayTracing(Ray(tp.P,Refl),fac*absor*(material->color*material->refl),weight*material->refl,depth+1,hash);
        else if(Refr.len2()<Const::eps) // 全反射
        {
            double k=material->refl+material->refr;
            res=res+rayTracing(Ray(tp.P,Refl),fac*absor*(material->color*k),weight*k,depth+1,hash);
        }else if(material->refl<Const::eps) // 全透射
        {
            res=res+rayTracing(Ray(tp.P,Refr),fac*absor*material->refr,weight*material->refr,depth+1,hash);
        }else
        {
            double kl=material->refl,kr=material->refr;
            if (1||Const::enable_fresnel) // Fresnel equations
            {
                double cosI=-tp.ray.D.dot(tp.N),cosT=sqrt(1-(1-cosI*cosI)/rindex/rindex);
                double r1=(cosI*rindex-cosT)/(cosI*rindex+cosT);
                double r2=(cosI-cosT*rindex)/(cosI+cosT*rindex);
                kl=(r1*r1+r2*r2)/2;kr=1-kl;
            }
            if (kl>Const::eps)res=res+rayTracing(Ray(tp.P,Refl),fac*absor*(material->color*kl),weight*kl,depth+1,hash);
            if (kr>Const::eps)res=res+rayTracing(Ray(tp.P,Refr),fac*absor*kr,weight*kr,depth+1,hash);
        }
    }
    return res;
}


