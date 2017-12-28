//
//  vec.cpp
//  Raytracing
//
//  Created by hta on 2017/11/20.
//  Copyright © 2017年 hta. All rights reserved.
//

#include "common.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>
using namespace std;

//inline Vec operator +(const Vec &A,const Vec &B){return Vec(A.x+B.x,A.y+B.y,A.z+B.z);}
//inline Vec operator -(const Vec &A,const Vec &B){return Vec(A.x-B.x,A.y-B.y,A.z-B.z);}
//inline Vec operator *(const Vec &A,const Vec &B){return Vec(A.y*B.z-A.z*B.y,A.z*B.x-A.x*B.z,A.x*B.y-A.y*B.x);}
//inline Vec operator *(const Vec &A,const double &k){return Vec(A.x*k,A.y*k,A.z*k);}
//inline Vec operator /(const Vec &A,const double &k){return Vec(A.x/k,A.y/k,A.z/k);}
//inline Vec operator -(const Vec &A){return Vec(-A.x,-A.y,-A.z);}
//inline double Vec::len2(){return x*x+y*y+z*z;}
//inline double Vec::len(){return sqrt(len2());}
//inline double Vec::dot(const Vec &A)const{return A.x*x+A.y*y+A.z*z;}
//inline Vec Vec::normalize(){double l=len(); return l<Const::eps?*this:*this/l;}
//inline Vec Vec::reflect(const Vec &p)const{return *this-p*(2*this->dot(p));}
//void Vec::input(std::stringstream &fin){fin>>x>>y>>z;}

//Color operator +(const Color &A,const Color &B){return Color(A.r+B.r,A.g+B.g,A.b+B.b);}
//Color operator -(const Color &A,const Color &B){return Color(A.r-B.r,A.g-B.g,A.b-B.b);}
//Color operator *(const Color &A,const Color &B){return Color(A.r*B.r,A.g*B.g,A.b*B.b);}
//Color operator *(const Color &A,const double &k){return Color(A.r*k,A.g*k,A.b*k);}
//Color operator /(const Color &A,const double &k){return Color(A.r/k,A.g/k,A.b/k);}
//void Color::input(std::stringstream &fin){fin>>r>>g>>b;}
/*Color::Color(Json::Value &color):r(0),g(0),b(0)
{
    if (color.isString())
    {
        //std::string s=color.asString();
        //sscanf(s.c_str(),"(%lf,%lf,%lf)",&r,&g,&b);
    }
}*/
