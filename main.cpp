//
//  main.cpp
//  Raytracing
//
//  Created by hta on 2017/11/20.
//  Copyright © 2017年 hta. All rights reserved.
//

#include <iostream>
#include "raytracer.h"
#include "ppm.h"
#include <ctime>
using namespace std;
int main(int argc, const char * argv[]) {
    /*Vec A(5,6,7);
    //cout<<A.len()<<" "<<A.len2()<<endl;
    //cout << "Hello, World!\n";
    Bmp Pic("picture/floor.bmp");
    Pic.output("picture/fl2.bmp");
    //Bmp pic;
    //pic.Input("/Users/apple/Desktop/hta/Courses/2017Autumn/图形学/hw2/Raytracing/Raytracing/picture/floor.bmp");
    //pic.Output("/Users/apple/Desktop/hta/Courses/2017Autumn/图形学/hw2/Raytracing/Raytracing/picture/fl2.bmp");
    return 0;*/
    clock_t T=clock();
    //Raytracer *raytracer=new Raytracer("cornell_box.txt");
    //raytracer->Run("cornell_box.bmp");
    ppm *ppmtracer=new ppm("../cornell_box_glass.txt");
    ppmtracer->Run("cornell_box_glass.bmp");
    cerr<<(clock()-T)/CLOCKS_PER_SEC<<endl;
    return 0;
}
