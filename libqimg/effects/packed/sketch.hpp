
//  Copyright 2021 Isoheptane
//  Filename    : sketch.hpp
//  Purpose     : A simple sketch effect
//  License     : MIT License

#ifndef _LIBQIMG_FXP_SKETCH_HPP_
#define _LIBQIMG_FXP_SKETCH_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../../fmat.hpp"
#include "../../fmc.hpp"
#include "../../multiThread.hpp"
#include "../convolution.hpp"
#include "../valueMap.hpp"
#include "../blend.hpp"
#include "../blur/gaussianBlur.hpp"

namespace libqimg::Effects::Sketch {

    void sketch(
        FMAT& source, 
        FMAT& target,
        MTEXEC_PARAMS
    ) {

        FMAT canvas = FMAT::copy(source);

        Convolution::convolute(canvas, Point(1, 1), 1, [](CONV_KERNFUNC_PARAMS) {
            if(dx == 0 && dy == 0) return -8.0f;
            else return 1.0f;
        }, TileMode::clamp, false, threadCount, taskName);

        valueMap(canvas, [](VALUEMAP_PARAMS) {
            return math::clamp(1.0f - input, 0.0f, 1.0f);
        });

        FMAT canvasBlured = FMAT::copy(canvas);

        Blur::gaussianBlur(canvasBlured, 80, 80);

        valueMap(canvasBlured, [](VALUEMAP_PARAMS) {
            return math::lerp(2.0f, 0.0f, input);
        });

        blend(canvas, canvasBlured, [](BLEND_PARAMS) {
            return bottom - top;
        });

        valueMap(canvasBlured, [](VALUEMAP_PARAMS) {
            return 0.0f + ((float)rand() / (float)RAND_MAX) * 0.2f;
        });

        Blur::gaussianBlur(canvasBlured, 200, 5);

        blend(canvas, canvasBlured, [](BLEND_PARAMS) {
            return bottom - top;
        });

        target.copyContent(canvas);

        canvas.dispose();
        canvasBlured.dispose();
    }

}

#endif