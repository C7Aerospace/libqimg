
//  Copyright 2021 Isoheptane
//  Filename    : normalMap.hpp
//  Purpose     : Create normal map from a 
//  License     : MIT License

#ifndef _LIBQIMG_FX_NORMALMAP_HPP_
#define _LIBQIMG_FX_NORMALMAP_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"

namespace libqimg::Effects::Normal {

    // Create a normal map from FloatMatrix(Height)
    void bumpToNormal(
        FMAT& source, 
        FMAT& xmat, FMAT& ymat, FMAT& zmat,
        float scale = 1.0f,
        MTEXEC_PARAMS
    ) {
        
        auto function = 
            [&xmat, &ymat, &zmat, scale]
            (FMAT_MTEXEC_PARAMS) {

            float diffX = (
                matrix.pixelAccess(current.x - 1, current.y) - 
                matrix.pixelAccess(current.x + 1, current.y)) * scale;
            float diffY = (
                matrix.pixelAccess(current.x, current.y - 1) - 
                matrix.pixelAccess(current.x, current.y + 1)) * scale;
            
            xmat(current)= 0.5f + diffX;
            ymat(current) = 0.5f - diffY;
            zmat(current) = 1.0f;
            
        };

        if(!MultiThread::multiThreadExecute(source, function, taskName, threadCount))
            source.parameterizedForeach(function);

    }

}

#endif