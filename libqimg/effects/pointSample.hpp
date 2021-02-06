
//  Copyright 2021 Isoheptane
//  Filename    : pointSample.hpp
//  Purpose     : Sample from a single point.
//  License     : MIT License

#ifndef _LIBQIMG_FX_POINTSAMPLE_HPP_
#define _LIBQIMG_FX_POINTSAMPLE_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"

namespace libqimg::Effects::Sample {

    // FMAT Point Resample
    // Interpolated Point Sample. MultiThreading is not recommended for resample effects.
    void pointSample(
        FMAT& source, 
        FMAT& target, 
        SampleMode::SampleMode sampleMode, 
        MTEXEC_PARAMS
    ) {
        auto function = [&source, &sampleMode](FMAT_MTEXEC_PARAMS) {
            matrix(current.x, current.y) = 
                source.sample(source.positionFloat(matrix.coordinate(current)), sampleMode);
        };

        if(!MultiThread::multiThreadExecute(target, function, taskName, threadCount))
            target.parameterizedForeach(function); 
    }
    // FMC Point Resample
    // Interpolated Point Sample. MultiThreading is not recommended for resample effects.
    void pointSample(
        FMC& source, 
        FMC& target, 
        SampleMode::SampleMode sampleMode, 
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < target.count(); i++)
            pointSample(source[i], target[i], sampleMode, threadCount, taskName);
    }

}

#endif