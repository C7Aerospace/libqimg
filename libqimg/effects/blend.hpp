
//  Copyright 2021 Isoheptane
//  Filename    : map.hpp
//  Purpose     : Convert one value to another one
//  License     : MIT License

#ifndef _LIBQIMG_FX_BLEND_HPP_
#define _LIBQIMG_FX_BLEND_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"

namespace libqimg::Effects {

    #define BLEND_PARAMS float bottom, float top

    // Convert every float into another float value, function parameters: (float& bottop, float& top)
    template <class Function>
    void blend(
        FMAT& bottom, 
        FMAT& top,
        FMAT& target, 
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        
        auto function = [&bottom, &top, &converter](FMAT_MTEXEC_PARAMS) {
            reference = converter(bottom(current), top(current));
        };
        
        if(!MultiThread::multiThreadExecute(target, function, taskName, threadCount))
            target.parameterizedForeach(function);

    }
    // Effect self
    // Convert every float into another float value, function parameters: (float& bottop, float& top)
    template <class Function>
    void blend(
        FMAT& bottom, 
        FMAT& top,
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        blend(bottom, top, bottom, converter, threadCount, taskName);
    }

    // Convert every float into another float value, function parameters: (float& bottop, float& top)
    template <class Function>
    void blend(
        FMC& bottom, 
        FMC& top,
        FMC& target, 
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < target.count(); i++)
            blend(bottom[i], top[i], target[i], converter, threadCount, taskName);
    }
    // Effect self
    // Convert every float into another float value, function parameters: (float& bottop, float& top)
    template <class Function>
    void blend(
        FMC& bottom, 
        FMC& top,
        const Function& converter,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < bottom.count(); i++)
            blend(bottom[i], top[i], bottom[i], converter, threadCount, taskName);
    }
    
}

#endif