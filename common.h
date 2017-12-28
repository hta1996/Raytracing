//
//  vec.h
//  Raytracing
//
//  Created by hta on 2017/11/20.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef common_h
#define common_h
#include <sstream>
#include "const.h"
using namespace std;

struct Vec2
{
    double x,y;
    Vec2():x(0),y(0){}
    Vec2(double x,double y):x(x),y(y){}
    inline Vec2 operator +(const Vec2 &B)const{return Vec2(x+B.x,y+B.y);}
    inline Vec2 operator -(const Vec2 &B)const{return Vec2(x-B.x,y-B.y);}
    inline Vec2 operator -()const{return Vec2(-x,-y);}
    inline Vec2 operator *(double k)const{return Vec2(x*k,y*k);}
    inline Vec2 operator /(double k)const{return Vec2(x/k,y/k);}
    inline double dot(const Vec2 &B)const{return x*B.x+y*B.y;}
    inline double len2()const{return x*x+y*y;}
    inline double len()const{return sqrt(len2());}
    inline double arg()const{return atan2(y,x);}
    inline Vec2 normalize()const{double l=len();return l<Const::eps?*this:*this/l;}
};
struct Vec
{
    double x,y,z;
    inline Vec(double _x=0,double _y=0,double _z=0):x(_x),y(_y),z(_z){}
    inline double len2()const{return x*x+y*y+z*z;}
    inline double len()const{return sqrt(len2());}
    inline double dot(const Vec &B)const{return B.x*x+B.y*y+B.z*z;}
    inline Vec normalize()const{double l=len();return l<Const::eps?*this:*this/l;}
    inline Vec reflect(const Vec &p)const{return *this-p*(2*this->dot(p));}
    inline Vec refract(const Vec &n,double rindex)const
    {
        double r=1/rindex,cosI=this->dot(n),cosT=1-r*r*(1-cosI*cosI);
        if(cosT>=0)return (*this)*r-n*(sqrt(cosT)+cosI*r);
        else return Vec();
    }
    inline Vec vertical()const
    {
        Vec v=(*this)*(Vec(0,0,1));
        if(v.len2()<Const::eps)v=(*this)*(Vec(1,0,0));
        return v.normalize();
    }
    inline Vec diffuse()const
    {
        double a=acos(sqrt(Const::rand01())),b=2*Const::pi*Const::rand01();
        Vec D(cos(b)*sin(a),sin(b)*sin(a),cos(a));
        Vec dx=vertical(),dy=(*this)*(dx);
        return dx*D.x+dy*D.y+operator*(D.z);
    }
    void input(std::stringstream &fin){fin>>x>>y>>z;}
    inline Vec operator +(const Vec &B)const{return Vec(x+B.x,y+B.y,z+B.z);}
    inline Vec operator -(const Vec &B)const{return Vec(x-B.x,y-B.y,z-B.z);}
    inline Vec operator *(const Vec &B)const{return Vec(y*B.z-z*B.y,z*B.x-x*B.z,x*B.y-y*B.x);};
    inline Vec operator *(const double &k)const{return Vec(x*k,y*k,z*k);}
    inline Vec operator /(const double &k)const{return Vec(x/k,y/k,z/k);}
    inline Vec operator -()const{return Vec(-x,-y,-z);}
    inline Vec2 toVec2()const{return Vec2(x,y);}
    void Min(const Vec &B){x=min(x,B.x);y=min(y,B.y);z=min(z,B.z);}
    void Max(const Vec &B){x=max(x,B.x);y=max(y,B.y);z=max(z,B.z);}
};

struct Color
{
    double r,g,b;
    inline Color(double _x=0,double _y=0,double _z=0):r(_x),g(_y),b(_z){}
    inline Color operator +(const Color &B)const{return Color(r+B.r,g+B.g,b+B.b);}
    inline Color operator -(const Color &B)const{return Color(r-B.r,g-B.g,b-B.b);}
    inline Color operator *(const Color &B)const{return Color(r*B.r,g*B.g,b*B.b);}
    inline Color operator *(const double &k)const{return Color(r*k,g*k,b*k);}
    inline Color operator /(const double &k)const{return Color(r/k,g/k,b/k);}
    void input(std::stringstream &fin){fin>>r>>g>>b;}
    inline Color confine()const{return Color(max(min(r,1.),0.),max(min(g,1.),0.),max(min(b,1.),0.));}
    inline double power()const{return (r+g+b)/3.;}
    //Color(Json::Value &color);
};

struct Ray
{
    Vec O,D;
    inline Ray():O(),D(){}
    //inline Ray(const Vec &o,const Vec &d):O(o),D(d.normalize()){}
    inline Ray(const Vec &o,const Vec &d):O(o),D(d){}
    inline Ray normalize()const{return Ray(O,D.normalize());}
    inline Vec get(double t)const{return O+D*t;}
    //Ray normalize(const Ray &r){return }
};

struct Photon
{
    Photon():O(),D(),power(0){}
    Photon(const Vec &o,const Vec &d,const Color &pow):O(o),D(d),power(pow){}
    Vec O,D;
    Color power;
};

#endif /* common_h */
