//
//  curve.cpp
//  Raytracing
//
//  Created by hta on 2017/12/16.
//  Copyright © 2017年 hta. All rights reserved.
//

#include <stdio.h>
#include "curve.h"

int Poly::count(LD l,LD r)
{
    int cnt=0;
    LD fl,fr,gl=0,gr=0;
    if(r-l<Const::ceps)return 0;
    for(int i=0;i<7;i++)
    {
        if(c[i][6-i]==0)break;
        fl=fr=0;
        for(int j=6-i;j>=0;j--)fl=fl*l+c[i][j],fr=fr*r+c[i][j];
        if(fabs(fl)>Const::teps)
        {
            if(fl*gl<0)cnt++;
            gl=fl;
        }
        if(fabs(fr)>Const::teps)
        {
            if(fr*gr<0)cnt--;
            gr=fr;
        }
    }
    return abs(cnt);
}

bool Poly::solveOne(LD l,LD r,LD &x)
{
    x=l;
    for(int i=0;i<Const::iterationT;i++)
    {
        LD F=f(x),D=df(x),dx;
        if(fabs(F)<Const::seps)return l<x-Const::ceps&&x<r-Const::ceps;
        if(fabs(D)<Const::ceps)break;
        dx=F/D;x-=dx;
        if(fabs(dx)<Const::ceps)break;
    }
    return fabs(x)<Const::seps&&l<x-Const::ceps&&x<r-Const::ceps;
}

void Poly::solveAll(LD l,LD r)
{
    if(r-l<Const::ceps)
    {
        if(fabs(f(l))<Const::seps)roots.push_back(l);
        return;
    }
    int cnt=count(l,r);
    if(!cnt)return;
    else if(cnt==1)
    {
        LD x;
        if(solveOne(l,r,x)){roots.push_back(x);return;}
    }
    LD mid=(l+r)/2;
    solveAll(l,mid);
    solveAll(mid,r);
}


