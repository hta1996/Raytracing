//
//  primitive.h
//  Raytracing
//
//  Created by hta on 2017/12/2.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef object_h
#define object_h
#include "bmp.h"
#include "scene.h"
#include "curve.h"
#include<cstdio>
#include<fstream>
#include<iostream>
#include<string>
#include<cmath>
using namespace std;
class Primitive;
class Light;

struct Collider
{
    const Primitive *primitive;
    const Light *light;
    Ray ray;
    Vec P,N;//交点，法向量
    double dist;//ray.O+dist*ray.D=P
    double u,v;//参数曲面
    bool internal;//是否从内部碰撞
    int sample;
    inline Collider():dist(1e9){}
    
    inline Collider(const Ray &ray,double t,const Vec &n,const Primitive *pri, bool inter,int sam=0)// 与物体相交
        :primitive(pri),light(nullptr),
        ray(Ray(ray.O,ray.D)),P(ray.get(t)),N(n.normalize()),
        dist(t*ray.D.len()),internal(inter){}
    inline Collider(const Ray &ray,double t,double u,double v,const Vec &n, const Primitive *pri, bool inter,int sam=0)// 与物体相交，包含参数
        :primitive(pri),light(nullptr),
        ray(Ray(ray.O,ray.D)),P(ray.get(t)),N(n.normalize()),
        dist(t*ray.D.len()),internal(inter),u(u),v(v){}
    inline Collider(const Ray &ray,double t,const Light *l,int sample)// 与光源相交
        :primitive(nullptr),light(l),
        ray(Ray(ray.O,ray.D)),P(ray.get(t)),
        dist(t*ray.D.len()),internal(false),
        sample(sample){}
    inline bool hit()const{ return dist < 1e8; }
    inline bool hitPrimitive()const{return primitive!=nullptr;}
    inline bool hitLight()const{return light!=nullptr;}
};

class Material
{
public:
    Color color,absor;//物品色彩，透明物品吸收的色光
    double refl,refr,diff,spec;//反射，折射，漫反射，镜面漫反射的百分比
    double rindex;//折射率
    Bmp *texture;
    Material():color(1,1,1),absor(0,0,0),diff(0.8),spec(0.2),refl(0),refr(0),rindex(1),texture(nullptr){}
    void input(std::string,std::stringstream &);
    bool hasTexture()const{return texture;}
    Color textureColor(double u,double v)const;
    inline double BRDF(const Vec &l,const Vec &n,const Vec &v)const
    {
        Vec r=l.reflect(n);
        return diff*l.dot(n)+spec*pow(v.dot(r),Const::PHONG_expo);
    }
};

class Primitive
{
protected:
    Material *material;
    Primitive *next;
    int sample;
public:
    Primitive(){sample=rand();next=NULL;material=new Material;}
    int getSample()const{return sample;}
    Material *getMaterial()const{return material;}
    Primitive *getNext()const{return next;}
    void setNext(Primitive *pri){next=pri;}
    virtual Collider collide(const Ray &ray)const=0;
    virtual void input(std::string,std::stringstream &);
    virtual Color textureColor(const Collider &tp)const=0;
    virtual std::string getType()const=0;
};

class Sphere:public Primitive
{
public:
    Sphere():Primitive(){}
    Sphere(const Vec &o,double r,const Material* m=nullptr);
    virtual std::string getType()const override{return "Sphere";}
    virtual void input(std::string arg,std::stringstream &fin)override
    {
        if(arg=="O:")O.input(fin);
        else if(arg=="R:")fin>>r;
        else if(arg=="Dx:")Dx.input(fin);
        else if(arg=="Dz:")Dz.input(fin);
        Primitive::input(arg,fin);
    }
    inline virtual Collider collide(const Ray &ray)const override
    {
        //Ray uray=Ray(ray.O,ray.D);
        Ray uray=ray.normalize();
        Vec l=O-ray.O,P;
        double tp=l.dot(uray.D),tp2=r*r-l.len2()+tp*tp,t1,t2;
        if(tp2<0)return Collider();
        tp2=sqrt(tp2);t1=tp-tp2;t2=tp+tp2;
        if(t1>Const::eps)return Collider(uray,t1,uray.get(t1)-O,this,false);
        if(t2>Const::eps)return Collider(uray,t2,O-uray.get(t2),this,true);
        return Collider();
    }
    inline virtual Color textureColor(const Collider &tp)const override
    {
        return Color(1,1,1);
    }
    void setAxis(const Vec &dz,const Vec &dx){Dz=dz;Dx=dx;}// 设置纹理坐标轴
private:
    Vec O;        // 球心
    double r;         // 半径
    Vec Dz,Dx; // 北极向量和 0 度经线方向
};

class Plane:public Primitive
{
public:
    Plane(const Vec &N,double d,const Material* m = nullptr);
    Plane():Primitive(){}
    virtual std::string getType()const override{return "Sphere";}
    virtual void input(std::string arg,std::stringstream &fin)override
    {
        if(arg=="N:")N.input(fin);
        else if(arg=="d:")fin>>d;
        else if(arg=="Dx:")Dx.input(fin);
        else if(arg=="Dy:")Dy.input(fin);
        else if(arg=="Do:")Do.input(fin);
        Primitive::input(arg,fin);//????
    }
    inline virtual Collider collide(const Ray &ray)const override
    {
        //Ray uray=Ray(ray.O,ray.D);
        Ray uray=ray.normalize();
        double c=N.dot(uray.D);
        if(fabs(c)<Const::eps)return Collider();
        double tp=N.dot(uray.O)+d,t=-tp/c;
        if (t<Const::eps)return Collider();
        if (tp>Const::eps)return Collider(uray,t,N,this,false);
        else return Collider(uray,t,-N,this,true);
    }
    inline virtual Color textureColor(const Collider &tp)const override
    {
        if(material->hasTexture())
        {
            double u=(tp.P-Do).dot(Dx)/Dx.len2(),v=(tp.P-Do).dot(Dy)/Dy.len2();
            //if(u<0)cerr<<u<<" "<<v<<endl;
            u-=floor(u);v-=floor(v);
            return material->textureColor(u,v);
        }
        return Color(1,1,1);
    }
private:
    Vec N;//法向量
    double d;// 平面距离原点的距离N.P+d=0
    Vec Do,Dx,Dy;
};

class Cylinder:public Primitive//竖直的圆柱
{
public:
    Cylinder(const Vec &_O,double _r,double _h):Primitive(),O(_O),r(_r),h(_h){}
    Cylinder():Primitive(){}
    virtual std::string getType()const override{return "Cylinder";}
    virtual void input(std::string arg,std::stringstream &fin)override
    {
        if(arg=="O:")O.input(fin);
        else if(arg=="R:")fin>>r;
        else if(arg=="H:")fin>>h;
        else if(arg=="ang:"){fin>>ang;ang=ang/180*Const::pi;}
        Primitive::input(arg,fin);//????
    }
    inline virtual Collider collide(const Ray &ray)const override
    {
        Ray uray=ray.normalize();
        Vec d3=uray.D;
        Vec2 d2(d3.x,d3.y),oc=Vec2(O.x-ray.O.x,O.y-ray.O.y);
        double t=-1;
        bool internal=O.z<ray.O.z+Const::eps&&ray.O.z<O.z+h+Const::eps&&oc.len2()<r*r+Const::eps;
        // 与两个底面求交
        if (std::abs(d3.z)>Const::eps)//????
        {
            double t1=(O.z-ray.O.z)/d3.z,t2=(O.z+h-ray.O.z)/d3.z,u1=0,u2=1;
            if(t1>t2)swap(t1,t2),swap(u1,u2);
            if(t1>Const::eps) // 和第一个底面相交
            {
                Vec p=uray.get(t1);
                Vec2 q=(p-O).toVec2();
                if(q.len2()<r*r+Const::eps)
                    return Collider(uray,t1,u1,q.arg(),Vec(0,0,-d3.z),this,internal);
            }else if(t2>Const::eps) //和第二个底面相交，且射线起点在圆柱体内
            {
                Vec p=uray.get(t2);
                Vec2 q=(p-O).toVec2();
                if(q.len2()<r*r+Const::eps&&internal)
                    return Collider(uray,t2,u2,q.arg(),Vec(0,0,-d3.z),this,internal);
            }
            else return Collider();// 若射线不和上下底面所在的平面相交，则无交点
        }
        // 若射线垂直于 xy 平面，则交点不会在圆柱面上
        if(d2.len2()<Const::eps&&t<0)return Collider();
        // 否则第一个交点为圆柱面
        double tca=oc.dot(d2.normalize()),thc2=r*r-oc.len2()+tca*tca;
        if(thc2>=0)
        {
            double thc=sqrt(thc2),t1=tca-thc,t2=tca+thc;
            if(t1>Const::eps)t=t1;else t=t2;
        }
        if(t>Const::eps)
        {
            t/=d2.len();
            Vec p=uray.get(t);
            if(O.z<p.z+Const::eps&&p.z<O.z+h+Const::eps)
                return Collider(uray,t,(p.z-O.z)/h,(p-O).toVec2().arg(),Vec(p.x-O.x,p.y-O.y,0)*(internal?-1:1),this,internal);
        }
        return Collider();
    }
    inline virtual Color textureColor(const Collider &tp)const override
    {
        return Color(1,1,1);
    }
private:
    Vec O;
    double r,h,ang;//底面半径，高，文理
};

class RotationBody:public Primitive
{
public:
    //RotationBody(const Vec& o, const Curve &curves={}, const Material* m = nullptr);
    //RotationBody(const Json::Value& object);
    RotationBody():Primitive(){h=r=0;cur.clear();textR.clear();stextR.clear();subCylinders.clear();sample.clear();}
    ~RotationBody()
    {
        if(cylinder)delete cylinder;
        for(auto c:subCylinders)delete c;
        subCylinders.clear();
    }
    virtual std::string getType()const override{return "RotationBody";}
    virtual Collider collide(const Ray& ray)const override
    {
        Ray uray=ray.normalize();
        Vec d=uray.D;
        int curve_id=0;
        Vec2 res(1e9,0);
        Collider coll=cylinder->collide(ray);
        if(!coll.hit())return Collider();
        for(int i=0;i<(int)cur.size();i++)
        {
            coll=subCylinders[i]->collide(ray);
            if(!coll.hit()||(!coll.internal&&coll.dist>res.x-Const::eps))continue;
            Vec o=ray.O-O;
            Vec2 w=d.toVec2(),q0,q1,q2,q3;
            // A.y^2 + B.y + C + D.x^2 = 0
            LD A=w.len2(),B=2*w.dot(o.toVec2())*d.z-2*o.z*A,C=Vec2(o.z*d.x-o.x*d.z,o.z*d.y-o.y*d.z).len2(),D=-d.z*d.z,a[7];
            // a0 + a1.u + a2.u^2 + a3.u^3 + a4.u^4 + a5.u^5 + a6.u^6 = 0
            cur[i].getEqn(q0,q1,q2,q3);
            a[0]=A*q0.y*q0.y+D*q0.x*q0.x+C+B*q0.y;
            a[1]=2*A*q0.y*q1.y+2*D*q0.x*q1.x+B*q1.y;
            a[2]=A*(q1.y*q1.y+2*q0.y*q2.y)+D*(q1.x*q1.x+2*q0.x*q2.x)+B*q2.y;
            a[3]=2*A*(q0.y*q3.y+q1.y*q2.y)+2*D*(q0.x*q3.x+q1.x*q2.x)+B*q3.y;
            a[4]=A*(2*q1.y*q3.y+q2.y*q2.y)+D*(2*q1.x*q3.x+q2.x*q2.x);
            a[5]=2*(A*q2.y*q3.y+D*q2.x*q3.x);
            a[6]=A*q3.y*q3.y+D*q3.x*q3.x;
            Poly poly(a);
            vector<double> roots=poly.findAll(0,1);
            for(auto u:roots)
            {
                double t = -1;
                Vec2 p=cur[i].f(u);
                if(fabs(d.z)>Const::eps)t=(p.y-o.z)/d.z;
                else
                {
                    Vec2 oc=-o.toVec2();
                    double tca=oc.dot(w),thc2=p.x*p.x-oc.len2()+tca*tca;
                    if(thc2>-Const::eps)
                    {
                        double thc=sqrt(thc2),A=tca-thc,B=tca+thc;
                        if(A>Const::eps)t=A;else t=B;
                    }
                }
                if(t>1e-4&&t<res.x-Const::eps)res=Vec2(t,u),curve_id=i;
            }
        }
        if(res.x<1e9-1)
        {
            Vec2 v=(uray.get(res.x)-O).toVec2().normalize(),dp=cur[curve_id].df(res.y);
            Vec n=Vec(dp.y*v.x,dp.y*v.y,-dp.x);
            if(n.dot(d)<Const::eps)
                return Collider(uray,res.x,curve_id+res.y,fmod(v.arg(),2*Const::pi),n,this,false,sample[curve_id]);
            else
                return Collider(uray,res.x,curve_id+res.y,fmod(v.arg(),2*Const::pi),-n,this,true,sample[curve_id]);
        }else return Collider();
    }
    virtual Color textureColor(const Collider &tp)const override
    {
        return Color(1,1,1);
    }
    virtual void input(std::string arg,std::stringstream &fin)override
    {
        if(arg=="O:"){O.input(fin);init();}
        else if(arg=="ang:"){fin>>ang;ang=ang/180*Const::pi;}
        else if(arg=="curve:"){Curve c;c.input(fin);cur.push_back(c);}
        else if(arg=="ratio:")
            for(auto c:cur)
            {
                double r;fin>>r;textR.push_back(r);
            }
        Primitive::input(arg,fin);//????
    }
    // 设置底面中心点
    void setO(const Vec &o){O=o;}
    // 曲面上一点 P(u, v)
    Vec P(int i,double u,double v) const;
private:
    //输入中O作为最后一个参数 ratio 在curve之后输入
    Vec O;                                                // 底面中心点
    vector<Curve> cur;                                            // 曲线
    double r,h,ang;                                     // 包围圆柱体的底面半径、高，纹理起点极角
    vector<double> textR,stextR; // 每个子曲面纹理的比例，比例前缀和
    vector<Cylinder*> subCylinders; // 子旋转面的包围圆柱体
    Cylinder *cylinder;          // 包围圆柱体
    vector<int> sample;      // 每个子旋转面都有标识符
    void init()
    {
        for(auto c:cur)
        {
            double tp=max(fabs(c.xmin),fabs(c.xmax));
            r=max(r,tp),h=max(h,c.ymax);
            subCylinders.push_back(new Cylinder(Vec(O.x,O.y,O.z+c.ymin),tp,c.ymax-c.ymin));
            sample.push_back(rand());
        }
        cylinder=new Cylinder(O,r,h);
        double s=0;
        for(double r:textR)stextR.push_back(s),s+=r;
    }
};


class Scene;

class Light
{
public:
    Light(){sample=rand();next=nullptr;}
    Light *getNext(){return next;}
    void setNext(Light *light){next=light;}
    int getSample()const{return sample;}
    Color getCol()const{return color;}
    double getPow()const{return power;}
    virtual Collider collide(const Ray& ray)const=0;
    virtual Vec getSource(double x=0,double y=0)const=0;
    virtual std::string getType()const=0;
    virtual void input(std::string,std::stringstream &);
    virtual Photon emit(double power)const=0;
protected:
    Color color;
    double power;
    int sample;
    Light *next;
};

class Pointlight:public Light
{
public:
    Pointlight():Light(){}
    inline virtual Collider collide(const Ray& ray)const override{return Collider();}
    inline virtual Vec getSource(double x=0,double y=0)const override{return O;}
    virtual std::string getType()const override{return "Point";}
    virtual void input(std::string arg,std::stringstream &fin)override
    {
        if(arg=="O:")O.input(fin);
        Light::input(arg,fin);
    }
    virtual Photon emit(double pow)const override
    {
        return Photon(O,Vec(2*Const::rand01()-1,2*Const::rand01()-1,2*Const::rand01()-1).normalize(),color*pow);
    }
private:
    Vec O;
};

class Arealight:public Light
{
public:
    Arealight():Light(){}
    inline virtual Collider collide(const Ray& ray)const override
    {
        //Ray uray=Ray(ray.O,ray.D);
        Ray uray=ray.normalize();
        double c=N.dot(uray.D);
        if(fabs(c)<Const::eps)return Collider();
        double tp=N.dot(O)-N.dot(uray.O),t=tp/c;
        if(t<Const::eps)return Collider();
        Vec p=uray.get(t)-O;
        if (fabs(p.dot(Dx))+Const::eps<Dx.len2()&&fabs(p.dot(Dy))+Const::eps<Dy.len2())
            return Collider(uray,t,this,this->getSample());
        else return Collider();
    }
    inline virtual Vec getSource(double x=0,double y=0)const override{return O+Dx*x+Dy*y;}///
    virtual std::string getType()const override{return "Area";}
    virtual void input(std::string arg,std::stringstream &fin)override
    {
        if(arg=="O:")O.input(fin);
        else if(arg=="N:")N.input(fin);
        else if(arg=="Dx:")Dx.input(fin);
        else if(arg=="Dy:")Dy.input(fin);
        Light::input(arg,fin);
    }
    virtual Photon emit(double pow)const override
    {
        return Photon(O+Dx*(2*Const::rand01()-1)+Dy*(2*Const::rand01()-1),N.diffuse(),color*pow);
    }
private:
    Vec O,N,Dx,Dy;
};

class Camera
{
public:
    Camera(){a=NULL;}
    ~Camera();
    void input(std::string arg,std::stringstream &fin);
    void setCol(int i,int j,const Color &col){a[i][j]=col;}
    Color getCol(int i,int j)const{return(0<=i&&i<W&&0<=j&&j<H)?a[i][j]:Color();}
    Ray emit(double x,double y)const{return Ray(O,D+Dw*(2.*x/W-1)+Dh*(2.*y/H-1));}///没单位化了方向！！
    Ray emitDOF(double x,double y)const
    {
        Vec P=O+emit(x,y).D.normalize()*focalLen;
        double dw=1,dh=1;
        while(dw*dw+dh*dh>1)
        {
            dw=Const::rand01()*2-1;
            dh=Const::rand01()*2-1;
        }
        Vec S=O+Dw.normalize()*aperture*dw+Dh.normalize()*aperture*dh;
        return Ray(S,P-S);
    }
    void print(const std::string &file)
    {
        Bmp* pic=new Bmp(W,H);
        for(int i=0;i<W;i++)
            for(int j=0;j<H;j++)pic->setCol(i,j,a[i][j].confine());
        pic->output(file);
    }
    int getW()const{return W;}
    int getH()const{return H;}
    Bmp* getBmp()const
    {
        Bmp* pic=new Bmp(W,H);
        for(int i=0;i<W;i++)
            for(int j=0;j<H;j++)pic->setCol(i,j,getCol(i,j));
        return pic;
    }
    void setBmp(const Bmp *pic)
    {
        for(int i=0;i<W;i++)
            for(int j=0;j<H;j++)setCol(i,j,pic->getCol(i,j));
    }
    double getAperture()const{return aperture;}
private:
    Vec O,Look,D,Up; // 相机位置、相机视线上任意一点、视线方向、上方向
    int W,H;                          // 分辨率
    double fovy;                         // 相机视野张角
    double aperture,focalLen;        // 光圈大小，焦距(默认为 O 和 Look 的距离)
    Vec Dw,Dh;                    // 视平面坐标系方向
    Color **a;// 颜色缓存
    //输入文件中，eye，look，focal_len的顺序读入,W最后
    void init();
};

class Scene
{
public:
    Scene()
    {
        camera=new Camera;
        priHead=NULL;
        lightHead=NULL;
    }
    void inputAmbientCol(std::string arg,std::stringstream &fin);
    Color getAmbientCol()const{return ambientCol;}
    void addPrimitive(Primitive *pri);
    void addLight(Light *light);
    Primitive* startPri()const{return priHead;}
    Light* startLight()const{return lightHead;}
    void createScene(std::string file);
    Collider findCollision(const Ray &ray)const;
    double calcShade(const Light *light,const Vec &p)
    {
        if(light->getType()=="Point")
        {
            Vec D=(light->getSource())-p;
            double dist=D.len();
            for (Primitive *itr=priHead;itr!=NULL;itr=itr->getNext())
            {
                Collider tp=itr->collide(Ray(p,D));
                if (tp.hit()&&tp.dist+Const::eps<dist)return 0;
            }
            return 1;
        }else
        {
            int T=Const::softShadowT,ret=T*T;
            for(int i=0;i<T;i++)
                for(int j=0;j<T;j++)
                {
                    double x=(i+0.5)*2/T-1,y=(j+0.5)*2/T-1;
                    //double xx=Const::randDouble()-1,yy=Const::randDouble()-1;
                    //xx/=2;yy/=2;
                    double xx=Const::rand01()*2-1,yy=Const::rand01()*2-1;
                    //cerr<<xx<<" "<<yy<<endl;
                    x+=xx/T;y+=yy/T;
                    Vec s=light->getSource(x,y),D=s-p;
                    double dist=D.len();
                    for (Primitive *itr=priHead;itr!=NULL;itr=itr->getNext())
                    {
                        Collider tp=itr->collide(Ray(p,D));
                        if (tp.hit()&&tp.dist+Const::eps<dist)
                        {
                            ret--;break;
                        }
                    }
                }
            return 1.*ret/T/T;
        }
    }
    Camera* getCamera()const{return camera;}
private:
    Camera *camera;
    Color ambientCol;
    Primitive *priHead;
    Light *lightHead;
};


#endif /* object_h */
