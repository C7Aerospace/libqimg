
//  Copyright 2021 Isoheptane
//  Filename    : areaSample.hpp
//  Purpose     : Sample from an area.
//  License     : MIT License

#ifndef _LIBQIMG_FX_AREASAMPLE_HPP_
#define _LIBQIMG_FX_AREASAMPLE_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"

namespace libqimg::Effects::Sample {

    // Averave Area Sample. MultiThreading is not recommended for resample effects.
    void areaSample(
        FMAT& source, 
        FMAT& target, 
        MTEXEC_PARAMS
    ) {
        
        auto function = [&source](FMAT_MTEXEC_PARAMS) {
            
            matrix(current) = source.average(
                PointF(
                    (float)current.x * (float)source.width() / (float)matrix.width(),
                    (float)current.y * (float)source.height() / (float)matrix.height()
                ),
                PointF(
                    (float)(current.x + 1) * (float)source.width() / (float)matrix.width(),
                    (float)(current.y + 1) * (float)source.height() / (float)matrix.height()
                )
            );
        };

        if(!MultiThread::multiThreadExecute(target, function, taskName, threadCount))
            target.parameterizedForeach(function); 
    }

    // Averave Area Sample. MultiThreading is not recommended for resample effects.
    void areaSample(
        FMC& source, 
        FMC& target, 
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < target.count(); i++)
            areaSample(source[i], target[i], threadCount, taskName);
    }

}

#endif