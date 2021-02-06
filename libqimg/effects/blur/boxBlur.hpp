
//  Copyright 2021 Isoheptane
//  Filename    : boxBlur.hpp
//  Purpose     : Average Box Blur
//  License     : MIT License

#ifndef _LIBQIMG_FX_BOXBLUR_HPP_
#define _LIBQIMG_FX_BOXBLUR_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../../fmat.hpp"
#include "../../fmc.hpp"
#include "../../multiThread.hpp"
#include "../convolution.hpp"

namespace libqimg::Effects::Blur {

    // Gaussian Blur
    void boxBlur(
        FMAT& source, 
        FMAT& target, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {

        FMAT canvas = FMAT(target.canvasSize());
        Convolution::convolute(source, canvas, Point(radiusX, 0), 1, [](CONV_KERNFUNC_PARAMS) {
            return 1.0f;
        }, edgeMode, true, threadCount, taskName);
        Convolution::convolute(canvas, target, Point(0, radiusY), 1, [](CONV_KERNFUNC_PARAMS) {
            return 1.0f;
        }, edgeMode, true, threadCount, taskName);
        canvas.dispose();

    }

    void boxBlur(
        FMAT& source, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT::copy(source);
        boxBlur(cache, source, radiusX, radiusY, edgeMode, threadCount, taskName);
        cache.dispose();
    }
    
    // Gaussian Blur
    void boxBlur(
        FMC& source, 
        FMC& target, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {

        FMAT canvas = FMAT(target.canvasSize());

        for(int i = 0; i < target.count(); i++) {
            Convolution::convolute(source[i], canvas, Point(radiusX, 0), 1, [](CONV_KERNFUNC_PARAMS) {
                return 1.0f;
            }, edgeMode, true, threadCount, taskName);
            Convolution::convolute(canvas, target[i], Point(0, radiusY), 1, [](CONV_KERNFUNC_PARAMS) {
                return 1.0f;
            }, edgeMode, true, threadCount, taskName);
        }

        canvas.dispose();

    }

    void boxBlur(
        FMC& source, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {
        FMC cache = FMC::copy(source);
        boxBlur(cache, source, radiusX, radiusY, edgeMode, threadCount, taskName);
        cache.dispose();
    }

}

#endif