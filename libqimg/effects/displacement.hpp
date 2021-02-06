
//  Copyright 2021 Isoheptane
//  Filename    : displacement.hpp
//  Purpose     : Displacement
//  License     : MIT License

#ifndef _LIBQIMG_FX_DISPLACEMENT_HPP_
#define _LIBQIMG_FX_DISPLACEMENT_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"

namespace libqimg::Effects {

    // Use FMAT as offset
    // Use offset to resample matrix
    void displace(
        FMAT& source,
        FMAT& offsetX, FMAT& offsetY,
        FMAT& target,
        PointF scale = PointF(1.0f, 1.0f),
        TileMode::TileMode edgeMode = TileMode::clamp,
        SampleMode::SampleMode sampleMode = SampleMode::bilinear,
        MTEXEC_PARAMS
    ) {

        auto function = 
            [&source, &offsetX, &offsetY, &scale, &edgeMode, &sampleMode]
            (FMAT_MTEXEC_PARAMS) {
                reference = source.sample(PointF(
                    (float)current.x + 0.5f + (offsetX(current) * scale.x),
                    (float)current.y + 0.5f + (offsetY(current) * scale.y)),
                sampleMode, edgeMode);
            };

        if(!MultiThread::multiThreadExecute(target, function, taskName, threadCount))
            target.parameterizedForeach(function);

    }
    // Self effect
    // Use offset to resample matrix
    void displace(
        FMAT& source,
        FMAT& offsetX, FMAT& offsetY,
        PointF scale = PointF(1.0f, 1.0f),
        TileMode::TileMode edgeMode = TileMode::clamp,
        SampleMode::SampleMode sampleMode = SampleMode::bilinear,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT::copy(source);
        displace(cache, offsetX, offsetY, source, scale, edgeMode, sampleMode, threadCount, taskName);
        cache.dispose();
    }
    // Use FMAT as offset
    // Use offset to resample matrix
    void displace(
        FMC& source,
        FMAT& offsetX, FMAT& offsetY,
        FMC& target,
        PointF scale = PointF(1.0f, 1.0f),
        TileMode::TileMode edgeMode = TileMode::clamp,
        SampleMode::SampleMode sampleMode = SampleMode::bilinear,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < target.count(); i++)
            displace(source[i], offsetX, offsetY, target[i], scale, edgeMode, sampleMode, threadCount, taskName);
    }
    // Self effect
    // Use offset to resample matrix
    void displace(
        FMC& source,
        FMAT& offsetX, FMAT& offsetY,
        PointF scale = PointF(1.0f, 1.0f),
        TileMode::TileMode edgeMode = TileMode::clamp,
        SampleMode::SampleMode sampleMode = SampleMode::bilinear,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT(source.canvasSize());
        for(int i = 0; i < source.count(); i++) {
            cache.copyContent(source[i]);
            displace(cache, offsetX, offsetY, source[i], scale, edgeMode, sampleMode, threadCount, taskName);
        }
        cache.dispose();
    }
    // Each channel has different offset FMAT
    // Use offset to resample matrix
    void displace(
        FMC& source,
        FMC& offsetX, FMC& offsetY,
        FMC& target,
        PointF scale = PointF(1.0f, 1.0f),
        TileMode::TileMode edgeMode = TileMode::clamp,
        SampleMode::SampleMode sampleMode = SampleMode::bilinear,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < target.count(); i++)
            displace(source[i], offsetX[i], offsetY[i], target[i], scale, edgeMode, sampleMode, threadCount, taskName);
    }
    // Self effect
    // Use offset to resample matrix
    void displace(
        FMC& source,
        FMC& offsetX, FMC& offsetY,
        PointF scale = PointF(1.0f, 1.0f),
        TileMode::TileMode edgeMode = TileMode::clamp,
        SampleMode::SampleMode sampleMode = SampleMode::bilinear,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < source.count(); i++)
            displace(source[i], offsetX[i], offsetY[i], scale, edgeMode, sampleMode, threadCount, taskName);
    }
}

#endif