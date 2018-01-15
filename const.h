//
//  const.h
//  Raytracing
//
//  Created by hta on 2017/11/30.
//  Copyright © 2017年 hta. All rights reserved.
//

#ifndef const_h
#define const_h
#include <cmath>
#include <cstdlib>
using namespace std;
typedef unsigned long long uLL;
typedef long double LD;
//#include "dist/json/json.h"
//#include "dist/json/json-forwards.h"
namespace Const {
    const double eps=1e-6;
    const LD ceps=1e-12;
    const LD seps=1e-16;
    const LD teps=1e-22;
    const double pi=acos(-1.0);
    const double PHONG_expo=50;
    const int softShadowT=5;
    const double output_refresh_interval=10.0;
    const int depth_of_field_samples=1;
    const int maxRaytracingDepth=20;
    const int maxPhotontracingDepth=10;
    const bool enable_fresnel=false;
    const double raytracing_min_weight=0.04;
    const int antiAliasingT=5;
    const int hashseed=1000000007;
    const int iterationT=21;
    const int threadT=50;
    const double initr2=0.2;//0.05;
    const double alpha=2.0/3;
    const int ppmIterT=15000;
    const int emitPhotonT=1000000;//10000;
    inline int randUInt()
    {
        #ifdef __linux
            return rand();
        #else
            return (rand() << 15) | rand();
        #endif
    }
    
    inline uLL randUInt64()
    {
        #ifdef __linux
            return (rand() << 31) | rand();
        #else
            return (((((rand() << 15) | rand()) << 15) | rand()) << 15) | rand();
        #endif
    }
    
    inline double rand01()
    {
        //#ifdef __linux
        return 1.0 * rand() / RAND_MAX;
        //#else
            return 1.0 * randUInt() / (1 << 31);
        //#endif
    }
}

#endif /* const_h */
