
//  Copyright 2021 Isoheptane
//  Filename    : convolution.hpp
//  Purpose     : Image convolution
//  License     : MIT License

#ifndef _LIBQIMG_FX_CONVOLUTION_HPP_
#define _LIBQIMG_FX_CONVOLUTION_HPP_

#include <cstring>

#include "libqimg_math.hpp"
#include "../fmat.hpp"
#include "../fmc.hpp"
#include "../multiThread.hpp"

namespace libqimg::Effects::Convolution {

    /*
        Default Kernel
    */

    // Convolution Kernel
    struct Kernel {
       private:
        Point radius;
        float* data;
       public:
        
        Kernel(Point radius):radius(radius) {
            int sx = radius.x * 2 + 1;
            int sy = radius.y * 2 + 1;
            data = new float[sx * sy];
            for(int i = 0; i < sx * sy; i++)
                data[i] = 0.0f;
        }

        Kernel(int x, int y):radius(Point(x, y)) {
            int sx = x * 2 + 1;
            int sy = y * 2 + 1;
            data = new float[sx * sy];
            for(int i = 0; i < sx * sy; i++)
                data[i] = 0.0f;
        }

        Kernel(const FMAT& mat) {

            int sx = mat.width() | 1;
            int sy = mat.height() | 1;
            
            radius = Point(sx / 2, sy / 2);
            data = new float[sx * sy];

            for(int y = 0; y < mat.height(); y++)
                for(int x = 0; x < mat.width(); x++)
                    data[y * sx + x] = mat.pixelAccess(x, y);
            
        }

        Point size() const { return radius; }

        void dispose() {
            delete[] data;
        }

        float& operator()(int x, int y) {
            return data[(y + radius.y) * (radius.x * 2 + 1) + (x + radius.x)];
        }

        float& operator()(Point pt) {
            return (*this)(pt.x, pt.y);
        }

        float access(int x, int y) const {
            return data[(y + radius.y) * (radius.x * 2 + 1) + (x + radius.x)];
        }

        float access(Point pt) const {
            return this->access(pt.x, pt.y);
        }

    };

    // Image matrix convolution
    void convolute(
        FMAT& source, 
        FMAT& target, 
        const Kernel& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        auto function = 
            [&source, &kernel, &edgeMode, average]
            (FMAT_PARAMETERIZED_FOREACH_PARAMS) {

                float sum = 0.0f, weight = 0.0f;
                for(int dx = -kernel.size().x; dx <= kernel.size().x; dx++)
                    for(int dy = -kernel.size().y; dy <= kernel.size().y; dy++) {
                        float cw = kernel.access(dx, dy);
                        sum += source(current.x + dx, current.y + dy) * cw;
                        weight += kernel.access(dx, dy);
                    }
                if(average)
                    matrix(current) = sum / weight;
                else
                    matrix(current) = sum;
            };
        
        if(!MultiThread::multiThreadExecute(target, function, taskName, threadCount))
            target.parameterizedForeach(function);
    }
    // Self effect
    // Image matrix convolution
    void convolute(
        FMAT& source, 
        const Kernel& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT::copy(source);
        convolute(cache, source, kernel, edgeMode, average, threadCount, taskName);
        cache.dispose();
    }

    // Image matrix convolution
    void convolute(
        FMC& source, 
        FMC& target, 
        const Kernel& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        for(int i = 0; i < target.count(); i++)
            convolute(source[i], target[i], kernel, edgeMode, average, threadCount, taskName);
    }
    // Self effect
    // Image matrix convolution
    void convolute(
        FMC& source,  
        const Kernel& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT(source.canvasSize());
        for(int i = 0; i < source.count(); i++) {
            cache.copyContent(source[i]);
            convolute(cache, source[i], kernel, edgeMode, average, threadCount, taskName);
        }
        cache.dispose();
    }

    // Image matrix convolution
    void convolute(
        FMC& source, 
        FMC& target, 
        const FMAT& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        Kernel realKernel = Kernel(kernel);
        for(int i = 0; i < target.count(); i++)
            convolute(source[i], target[i], realKernel, edgeMode, average, threadCount, taskName);
        realKernel.dispose();
    }

    #define CONV_KERNFUNC_PARAMS int dx, int dy

    // Image convolution using kernel function
    template <class KernelFunction>
    void convolute(
        FMAT& source, 
        FMAT& target,
        Point size,
        int step,
        const KernelFunction& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        auto function = 
            [&source, &size, step, &kernel, &edgeMode, average]
            (FMAT_PARAMETERIZED_FOREACH_PARAMS) {

                float sum = 0.0f, weight = 0.0f;
                for(int dy = -size.y; dy <= size.y; dy += step)
                    for(int dx = -size.x; dx <= size.x; dx += step) {
                        float cw = kernel(dx, dy);
                        sum += source.pixelAccess(current.x + dx, current.y + dy, edgeMode) * cw;
                        weight += kernel(dx, dy);
                    }
                if(average)
                    matrix(current) = sum / weight;
                else
                    matrix(current) = sum;
            };
        
        if(!MultiThread::multiThreadExecute(target, function, taskName, threadCount))
            target.parameterizedForeach(function);
    }
        // Self effect
    // Image matrix convolution
    template <class KernelFunction>
    void convolute(
        FMAT& source, 
        Point size,
        int step,
        const KernelFunction& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT::copy(source);
        convolute(cache, source, size, step, kernel, edgeMode, average, threadCount, taskName);
        cache.dispose();
    }

    // Image convolution using kernel function
    template <class KernelFunction>
    void convolute(
        FMC& source, 
        FMC& target, 
        Point size,
        int step,
        const KernelFunction& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        for(int ch = 0; ch < target.count(); ch++)
           convolute(source[ch], target[ch], size, step, kernel, edgeMode, average, threadCount, taskName);
    }
    // Self effect
    // Image convolution using kernel function
    template <class KernelFunction>
    void convolute(
        FMC& source, 
        Point size,
        int step,
        const KernelFunction& kernel, 
        TileMode::TileMode edgeMode = TileMode::clamp,
        bool average = true,
        MTEXEC_PARAMS
    ) {
        FMAT cache = FMAT(source.canvasSize());
        for(int ch = 0; ch < source.count(); ch++) {
            cache.copyContent(source[ch]);
            convolute(cache, source[ch], size, step, kernel, edgeMode, average, threadCount, taskName);
        }
        cache.dispose();
    }

    /*
        Deformable Kernel
    */

    // Deformable Kernel, needs three channels
    struct DeformableKernel {
       private:
        Point radius;
       public:
        Kernel weight;
        Kernel deformX;
        Kernel deformY;

        DeformableKernel(Point radius):
            radius(radius), 
            weight(Kernel(radius)), 
            deformX(Kernel(radius)), 
            deformY(Kernel(radius)) {}

        DeformableKernel(int x, int y):
            radius(Point(x, y)), 
            weight(Kernel(Point(x, y))), 
            deformX(Kernel(Point(x, y))), 
            deformY(Kernel(Point(x, y))) {}

        DeformableKernel(FMAT& weight, FMAT& deformX, FMAT& deformY):
            weight(Kernel(weight)),
            deformX(Kernel(deformX)),
            deformY(Kernel(deformY)) {
            radius = this->weight.size();
        }

        // Return the offset value
        inline PointF offset(Point pt) {
            return PointF(deformX(pt), deformY(pt));
        }
        
        // Return the offseted value
        inline PointF transform(Point pt) {
            return PointF(static_cast<float>(pt.x), static_cast<float>(pt.y)) + offset(pt);
        }

    };

}

#endif