//
//  ppm.cpp
//  Raytracing
//
//  Created by hta on 2017/12/17.
//  Copyright © 2017年 hta. All rights reserved.
//

#include <stdio.h>
#include "ppm.h"
#include <mutex>
#include <thread>

void Photontracer::emit(int photonT)
{
    double powerT=0;
    int tot=0,threadT=Const::threadT;
    for(Light *itr=scene->startLight();itr!=NULL;itr=itr->getNext())powerT+=itr->getPow();
    mutex lock;
    vector<thread> a;a.clear();
    for(int i=0;i<threadT;i++)
    {
        double tp=powerT/(photonT/threadT);
        auto t=[this,powerT,tp,&tot,&lock]()
        {
            for(Light *itr=scene->startLight();itr!=NULL;itr=itr->getNext())
                for(double i=itr->getPow();i>0;i-=tp)
                {
                    photonTracing(itr->emit(powerT),1);
                    lock.lock();
                    if(++tot%100==0)
                        cout << "Emitted " << tot << " photons." << std::endl;
                    lock.unlock();
                }
        };
        a.push_back(thread(t));
    }
    for(int i=0;i<threadT;i++)a[i].join();
}

void Photontracer::photonTracing(Photon p,int depth)
{
    if(depth>Const::maxPhotontracingDepth)return;
    Collider tp=scene->findCollision(Ray(p.O,p.D));
    if(!tp.hit()||tp.hitLight())return;
    p.O=tp.P;
    const Primitive *pri=tp.primitive;
    const Material *mat=pri->getMaterial();
    if(mat->diff>Const::eps)map->modify(p);
    Color cd=mat->color*pri->textureColor(tp),ct(1, 1, 1);
    if(tp.internal) // 透明材质的颜色过滤
    {
        Color absor=mat->absor*(-tp.dist);
        absor=Color(exp(absor.r),exp(absor.g),exp(absor.b));
        cd=cd*absor;ct=ct*absor;
    }
    double ran=Const::rand01();
    double pd=(mat->diff+mat->spec)*cd.power();
    double ps=mat->refl*cd.power();
    double pt=mat->refr*ct.power();
    if(ran<pd) // 漫反射
    {
        p.D=tp.N.diffuse();
        p.power=p.power*cd/cd.power();
        photonTracing(p,depth+1);
    }else if(ran<pd+ps) // 镜面反射
    {
        p.D=tp.ray.D.reflect(tp.N);
        p.power=p.power*cd/cd.power();
        photonTracing(p,depth+1);
    }else if(ran<pd+ps+pt) // 透射
    {
        double rindex=(!tp.internal)?mat->rindex:1/mat->rindex;
        p.D=tp.ray.D.refract(tp.N,rindex);
        p.power=p.power*ct/ct.power();
        photonTracing(p,depth+1);
    }
    //if(ran<pd+ps+pt)photonTracing(p,depth+1);
}

void Map::update()
{
    cout<<"update!"<<endl;
    for(auto &p:point)
    {
        if(!p.m)continue;
        double k=(p.n+p.m*Const::alpha)/(p.n+p.m);
        p.r2*=k;p.flux=p.flux*k;
        p.n+=p.m*Const::alpha;
        p.m=0;
    }
    cout<<"rebuild!"<<endl;
    build(root);
}

void Map::modify(const Photon &p){modify(root,p);}

void Map::modify(Map::Ttree *x,const Photon &p)
{
    if(!x)return;
    if(p.O.x<x->L.x||p.O.x>x->R.x||p.O.y<x->L.y||p.O.y>x->R.y||p.O.z<x->L.z||p.O.z>x->R.z)return;
    if((p.O-x->p->O).len2()<=x->p->r2)x->p->update(p);
    modify(x->l,p);
    modify(x->r,p);
}

void Map::Ttree::pushup()
{
    double t=sqrt(p->r2);
    Vec T=Vec(t,t,t);
    L=p->O-T;R=p->O+T;
    if(l)L.Min(l->L),R.Max(l->R);
    if(r)L.Min(r->L),R.Max(r->R);
}

void Map::build(Map::Ttree *x)
{
    if(!x)return;
    build(x->l);
    build(x->r);
    x->pushup();
}

Map::Ttree* Map::build(int l,int r)
{
    if(l>=r)return nullptr;
    int mid=(l+r)/2,d;
    Vec A(0,0,0),B(0,0,0);
    for(int i=l;i<r;i++)A=A+point[i].O;
    A=A/(r-l);
    for(int i=l;i<r;i++)
        B.x+=(point[i].O.x-A.x)*(point[i].O.x-A.x),
        B.y+=(point[i].O.y-A.y)*(point[i].O.y-A.y),
        B.z+=(point[i].O.z-A.z)*(point[i].O.z-A.z);
    if(B.x>B.y&&B.x>B.z)d=0;
    else if(B.y>B.x&&B.y>B.z)d=1;
    else d=2;
    Ttree *x=tree+mid;
    nth_element(point.begin()+l,point.begin()+mid,point.begin()+r,[&](const Tpoint &x,const Tpoint &y)
                {
                    if(d==0)return x.O.x<y.O.x;
                    else if(d==1)return x.O.y<y.O.y;
                    else return x.O.z<y.O.z;
                });
    x->p=&point[mid];
    x->l=build(l,mid);
    x->r=build(mid+1,r);
    x->pushup();
    return x;
}

void Map::build()
{
    if(tree)delete[]tree;
    tree=new Ttree[n=point.size()];
    cout<<"Hit point: "<<n<<endl;
    root=build(0,n);
}

void ppm::Run(const std::string &file)
{
    map=new Map();
    flag=1;
    Raytracer::Run(file);
    flag=0;
    cout<<"Eye tracing..."<<endl;
    for(int i=0;i<W;i++)
        for(int j=0;j<H;j++)
        {
            if(!j&&i%10==0)cout<<"Eye tracing: column "<<i<<std::endl;
            curx=i,cury=j;
            if(vis[i][j])camera->setCol(i,j,calcECol(i,j,&hash[i][j]));
            else camera->setCol(i, j, calcCol(i,j,&hash[i][j]));
        }
    map->build();
    camera->print(("pre"+file).c_str());
    Bmp *pic=camera->getBmp();
    cout<<"Start iteration..."<<endl;
    Photontracer* PT=new Photontracer(scene,map);
    for(int i=0,photoT=0;i<Const::ppmIterT;i++)
    {
        cout<<"Round "<<i<<":"<<endl;
        PT->emit(Const::emitPhotonT);
        map->update();photoT+=Const::emitPhotonT;
        camera->setBmp(pic);
        for(auto p=map->startPoint();p!=map->endPoint();p++)
        {
            Color c=camera->getCol(p->x,p->y)+p->color*p->flux*(1/Const::pi/p->r2/photoT);
            camera->setCol(p->x,p->y,c);
        }
        camera->print(file.c_str());
    }
}
Color ppm::calcIllumination(const Collider &tp,const Material* mat,const Color &fac)const
{
    if (flag)
        return Raytracer::calcIllumination(tp,mat,fac);
    Tpoint p;
    p.O=tp.P;p.N=tp.N;p.D=tp.ray.D;
    p.mat=mat;
    p.color=mat->color*tp.primitive->textureColor(tp)*fac*mat->diff;
    p.r2=Const::initr2;
    p.x=curx;p.y=cury;
    map->insertPoint(p);
    return Color();
    /*if(Const::ppm)return Color();
    else return Raytracer::calcIllumination(tp,mat,fac);*/
}

