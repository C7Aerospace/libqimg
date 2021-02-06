
//  Copyright 2021 Isoheptane
//  Filename    : gaussianBlur.hpp
//  Purpose     : Gaussian Blur
//  License     : MIT License

#ifndef _LIBQIMG_FX_GAUSSIANBLUR_HPP_
#define _LIBQIMG_FX_GAUSSIANBLUR_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../../fmat.hpp"
#include "../../fmc.hpp"
#include "../../multiThread.hpp"
#include "../convolution.hpp"

namespace libqimg::Effects::Blur {

    /*
        Gaussian Blur: exp(-x^2)
    */
    float gaussianBlurEdge = 2.0f;
    // Gaussian Blur
    void gaussianBlur(
        FMAT& source, 
        FMAT& target, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {
        // Create canvas for process image cache
        FMAT canvas = FMAT(target.canvasSize());
        // X Axis Convolution
        {
            auto kernelX = Convolution::Kernel(radiusX, 0);
            for(int offset = -radiusX; offset <= radiusX; offset++)
                kernelX(offset, 0) = expf(-gaussianBlurEdge * (float)offset * (float)offset / ((float)radiusX + 0.5f));
            Convolution::convolute(source, canvas, kernelX, edgeMode, true, threadCount, taskName);
            kernelX.dispose();
        }
        // Y Axis Convolution
        {
            auto kernelY = Convolution::Kernel(0, radiusY);
            for(int offset = -radiusY; offset <= radiusY; offset++)
                kernelY(0, offset) = expf(-gaussianBlurEdge * (float)offset * (float)offset / ((float)radiusY + 0.5f));
            Convolution::convolute(canvas, target, kernelY, edgeMode, true, threadCount, taskName);
            kernelY.dispose();
        }
        // Clear cache
        canvas.dispose();
    }
    // Gaussian Blur
    void gaussianBlur(
        FMAT& source, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT::copy(source);
        gaussianBlur(cache, source, radiusX, radiusY, edgeMode, threadCount, taskName);
        cache.dispose();
    }

    // Gaussian Blur
    void gaussianBlur(
        FMC& source, 
        FMC& target, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {
        // Create canvas for process image cache
        FMAT canvas = FMAT(target.canvasSize());
        // Precompute Kernel
        auto kernelX = Convolution::Kernel(radiusX, 0);
        for(int offset = -radiusX; offset <= radiusX; offset++)
                kernelX(offset, 0) = expf(-gaussianBlurEdge * (float)offset * (float)offset / ((float)radiusX + 0.5f));
        auto kernelY = Convolution::Kernel(0, radiusY);
            for(int offset = -radiusY; offset <= radiusY; offset++)
                kernelY(0, offset) = expf(-gaussianBlurEdge * (float)offset * (float)offset / ((float)radiusY + 0.5f));

        for(int i = 0; i < target.count(); i++) {
            Convolution::convolute(source[i], canvas, kernelX, edgeMode, true, threadCount, taskName);
            Convolution::convolute(canvas, target[i], kernelY, edgeMode, true, threadCount, taskName);
        }
        // Clear Cache
        kernelX.dispose();
        kernelY.dispose();
        canvas.dispose();
    }
    // Gaussian Blur
    void gaussianBlur(
        FMC& source, 
        int radiusX, 
        int radiusY, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        MTEXEC_PARAMS
    ) {
        FMC cache = FMC::copy(source);
        gaussianBlur(cache, source, radiusX, radiusY, edgeMode, threadCount, taskName);
        cache.dispose();
    }

}

#endif