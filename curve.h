//
//  curve.h
//  Raytracing
//
//  Created by hta on 2017/12/16.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef curve_h
#define curve_h

#include "const.h"
#include "common.h"
#include <vector>
#include <cstring>
using namespace std;

struct Poly
{
public:
    inline Poly(const LD _[7])
    {
        for(int i=0;i<deg;i++)a[i]=_[i];
        memset(c,0,sizeof(c));
        for(int i=0;i<7;i++)
        {
            c[0][i]=a[i];
            if(i<6)c[1][i]=(i+1)*a[i+1];
        }
        for(int i=2;i<7;i++)
        {
            LD a=c[i-2][8-i]/c[i-1][7-i],b=(c[i-2][7-i]-a*c[i-1][6-i])/c[i-1][7-i];
            for(int j=0;j<7-i;j++)c[i][j]=(j?a*c[i-1][j-1]:0)+b*c[i-1][j]-c[i-2][j];
            if(fabs(c[i][6-i])<Const::teps)
            {
                for(int j=0;j<7-i;j++)c[i][j]=0;
                break;
            }
        }
    }
    inline LD f(LD x)const
    {
        LD r=0;
        for(int i=6;i>=0;i--)r=r*x+a[i];
        return r;
    }
    LD df(LD x)
    {
        LD r=0;
        for (int i=5;i>=0;i--)r=r*x+(i+1)*a[i+1];
        return r;
    }
    int count(LD l,LD r);
    vector<double> findAll(double l, double r)
    {
        roots.clear();
        solveAll(l,r);
        if(fabs(f(l))<Const::seps)roots.push_back(l);
        if(fabs(f(r))<Const::seps)roots.push_back(r);
        return roots;
    }
private:
    static const int deg=7;
    LD a[deg],c[deg][deg];
    vector<double> roots;
    bool solveOne(LD l,LD r,LD &x);
    void solveAll(LD l,LD r);
};

struct Curve
{
    //Curve(const Vec2 &p0,const Vec2 &p1,const Vec2 &p2,const Vec2 &p3);
    void input(std::stringstream &fin)
    {
        fin>>P0.x>>P0.y>>P1.x>>P1.y>>P2.x>>P2.y>>P3.x>>P3.y;
        xmin=min(P0.x,min(P1.x,min(P2.x,P3.x)));
        xmax=max(P0.x,max(P1.x,max(P2.x,P3.x)));
        ymin=min(P0.y,min(P1.y,min(P2.y,P3.y)));
        ymax=max(P0.y,max(P1.y,max(P2.y,P3.y)));
    }
    Vec2 f(double t)const{return P0*((1-t)*(1-t)*(1-t))+P1*(3*(1-t)*(1-t)*t)+P2*(3*(1-t)*t*t)+P3*(t*t*t);}
    Vec2 df(double t)const
    {
        double d=3*t*t,a=-d+6*t-3,b=d*3-12*t+3,c=-a-b-d;
        return P0*a+P1*b+P2*c+P3*d;
    }
    void getEqn(Vec2 &c0,Vec2 &c1,Vec2 &c2,Vec2 &c3)const{c0=P0;c1=(P1-P0)*3;c2=(P2-P1*2+P0)*3;c3=P3-(P2-P1)*3-P0;}
    Vec2 P0,P1,P2,P3;
    double xmin,xmax,ymin,ymax;// 包围盒
};


#endif /* curve_h */
