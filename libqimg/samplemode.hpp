
//  Copyright 2021 Isoheptane
//  Filename    : samplemode.hpp
//  Purpose     : Support Basic Sample Mode selection
//  License     : MIT License

#ifndef _LIBQIMG_SAMPLEMODE_HPP_
#define _LIBQIMG_SAMPLEMODE_HPP_

#include "libqimg_math.hpp"
#include "point.hpp"

namespace libqimg::SampleMode {

    // Sample Mode selection
    enum SampleMode {
        nearest = 0,
        bilinear = 1
    };
    
    float nearestSample(float lu, float ru, float ld, float rd, PointF t) {
        Point p = Point(t);
        if(p.x) return p.y ? rd : ru;
        else return p.y ? ld : lu;
    }

    float bilinearSample(float lu, float ru, float ld, float rd, PointF t) {
        float u = math::lerp(lu, ru, t.x);
        float d = math::lerp(ld, rd, t.x);
        return math::lerp(u, d, t.y);
    }

}

#endif