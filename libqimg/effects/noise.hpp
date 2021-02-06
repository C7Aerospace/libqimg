
//  Copyright 2021 Isoheptane
//  Filename    : noise.hpp
//  Purpose     : Generate Noise
//  License     : MIT License

#ifndef _LIBQIMG_FX_NOISE_HPP_
#define _LIBQIMG_FX_NOISE_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"
#include "valueMap.hpp"

namespace libqimg::Effects {

    // FMAT noise
    // Create random noise
    void noise(
        FMAT& source, 
        float min,
        float max,
        int seed,
        MTEXEC_PARAMS
    ) {

        srand(seed);
        auto function = [&min, &max](FMAT_MTEXEC_PARAMS) {
            reference = math::lerp(min, max, (float)rand() / (float)RAND_MAX);
        };
        
        if(!MultiThread::multiThreadExecute(source, function, taskName, threadCount))
            source.parameterizedForeach(function);

    }
    // Generate noise on each channel
    // Create random noise
    void noise(
        FMC& source, 
        float min,
        float max,
        int seed,
        bool singleColor = false,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < source.count(); i++)
            noise(source[i], min, max, singleColor ? seed : seed + i, threadCount, taskName);
    }
    
}

#endif