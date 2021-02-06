
//  Copyright 2021 Isoheptane
//  Filename    : valueMap.hpp
//  Purpose     : Convert one value to another one
//  License     : MIT License

#ifndef _LIBQIMG_FX_VALUEMAP_HPP_
#define _LIBQIMG_FX_VALUEMAP_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"

namespace libqimg::Effects {

    #define VALUEMAP_PARAMS float input
    // ValueMap for FMAT
    // Convert every float into another float value, function parameters: (float input)
    template <class Function>
    void valueMap(
        FMAT& source, 
        FMAT& target, 
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        
        auto function = [&source, &converter](FMAT_MTEXEC_PARAMS) {
            reference = converter(source(current));
        };
        
        if(!MultiThread::multiThreadExecute(target, function, taskName, threadCount))
            target.parameterizedForeach(function);

    }
    // Self effect
    // Convert every float into another float value, function parameters: (float input)
    template <class Function>
    void valueMap(
        FMAT& source, 
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        valueMap(source, source, converter, threadCount, taskName);
    }
    // ValueMap for FMC
    // Convert every float into another float value, function parameters: (float& input)
    template <class Function>
    void valueMap(
        FMC& source,
        FMC& target,
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < target.count(); i++)
            valueMap(source[i], target[i], converter, threadCount, taskName);
    }
    // Self effect
    // Convert every float into another float value, function parameters: (float& input)
    template <class Function>
    void valueMap(
        FMC& source,
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < source.count(); i++)
            valueMap(source[i], converter, threadCount, taskName);
    }
    
}

#endif