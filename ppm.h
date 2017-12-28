//
//  ppm.h
//  Raytracing
//
//  Created by hta on 2017/12/17.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef ppm_h
#define ppm_h
#include "object.h"
#include "raytracer.h"

struct Tpoint
{
    Vec O,N,D;      // 交点位置，法向，视线方向
    const Material* mat;
    Color color,flux;  // 材质原始颜色,光通量
    double r2;  // 采样半径的平方
    int n,m,x,y;   // 累计光子数，本轮发射累计光子数,像素点位置
    Tpoint():n(0),m(0),flux(){}
    void update(const Photon& p)
    {
        m++;
        if(p.D.dot(N)<0)flux=flux+p.power*mat->BRDF(-p.D,N,D);// 小于 -Const::eps
    }
};

class Map
{
public:
    
    Map():tree(nullptr),n(0){point.clear();}
    ~Map();
    
    const Tpoint &getP(const int &x)const{return point[x];}
    void insertPoint(const Tpoint &p){point.push_back(p);}
    void modify(const Photon& p);
    void build();
    void update();
    vector<Tpoint>::const_iterator startPoint()const{return point.begin();}
    vector<Tpoint>::const_iterator endPoint()const{return point.end();}
    int n;
private:
    struct Ttree
    {
        Ttree():p(nullptr){}
        Ttree(Tpoint *_p):p(_p){}
        //void init(){l=p->o-p->r;r=p->o+p->r;}
        void pushup();
        //{ init(); if (lc) l.updmin(lc->l), r.updmax(lc->r); if (rc) l.updmin(rc->l), r.updmax(rc->r); }
        Vec L,R;
        Tpoint *p;
        Ttree *l, *r;
    };
    Ttree *tree,*root;
    vector<Tpoint> point;
    Ttree *build(int l, int r);
    void build(Ttree *x);
    // 找所有距离不超过 sqrt(r2) 的 hit point
    void modify(Ttree *x,const Photon &p);
};


class Photontracer
{
public:
    Photontracer(Scene *_scene,Map *_map):map(_map),scene(_scene){}
    void emit(int tot);
private:
    Map *map;
    Scene *scene;
    void photonTracing(Photon p,int depth);
};

class ppm:public Raytracer
{
public:
    ppm(std::string file):Raytracer(file){}
    virtual void Run(const std::string &file)override;
private:
    Map* map;
    bool marking;
    virtual Color calcIllumination(const Collider &tp,const Material* material,const Color &fac)const override;
    void initHitPoint();
};

#endif /* ppm_h */
