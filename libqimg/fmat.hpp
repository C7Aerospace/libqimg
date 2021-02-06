
//  Copyright 2021 Isoheptane
//  Filename    : fmat.hpp
//  Purpose     : Type of FloatPointMatrix
//  License     : MIT License

#ifndef _LIBQIMG_FMAT_HPP_
#define _LIBQIMG_FMAT_HPP_

#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>

#include "libqimg_debuglog.hpp"
#include "libqimg_math.hpp"
#include "point.hpp"
#include "tilemode.hpp"
#include "samplemode.hpp"

namespace libqimg {

    /*
        Float Point Matrix File Structure (.fmat)
        0x0000  int32   Signature 80 72 7F A4
        0x0004  int32   Width
        0x0008  int32   Height
        ......  float   Data

        In the end of this file, you can write anything you want.
        Program won't read the tail of the file.

    */

    float _LIBQIMG_FMAT_SAFEADDRESS = 0.0f;
    const int FMAT_SIGNATURE = 0x80797FA4;
    // Float Point Matrix
    class FloatPointMatrix {
      private:
        bool openSucceed = false;
        Point size;
        float* dataptr = nullptr;
      public:
      
        // Initialize a collection by size.
        FloatPointMatrix(Point size):size(size) {
            dataptr = new float[size.x * size.y];
            openSucceed = true;
        }

        // Initialize a collection by size.
        FloatPointMatrix(int sizeX, int sizeY):size(Point(sizeX, sizeY)) {
            dataptr = new float[sizeX * sizeY];
            openSucceed = true;
        }

        // Read matrix from file
        FloatPointMatrix(std::string filename) {

#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : Opening file \"%s\"...\n", 
                filename.data(), 
                filename.data());
#endif
            auto file = std::ifstream(filename, std::ios::in | std::ios::binary);
            if(!file) {
#ifdef LIBQIMG_SHOWLOG
                printf("[FMAT \"%s\" ] : Cannot open file \"%s\".\n", 
                    filename.data(), 
                    filename.data());
#endif
                return;
            }
            // Check file signature
            int fileSignature;
            file.read((char*)&fileSignature, 4);
            if(fileSignature != FMAT_SIGNATURE) {
#ifdef LIBQIMG_SHOWLOG
                printf("[FMAT \"%s\" ] : \"%s\" signature incorrect.\n", filename.data(), filename.data());
#endif
                return;
            }
            // Readin data
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : Reading metadata...\n", filename.data());
#endif
            file.read((char*)&size, 8);
            dataptr = new float[size.x * size.y];
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] :  -> Resolution: %dx%d\n",
                filename.data(),
                size.x,
                size.y);
#endif
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : Reading matrix data...\n", filename.data());
#endif
            for(int y = 0; y < size.y; y++)
                for(int x = 0; x < size.x; x++)
                    file.read((char*)&dataptr[y * size.x + x], 4);
            file.close();
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : File \"%s\" loaded successfully.\n", 
                filename.data(), 
                filename.data());
#endif
            openSucceed = true;
        }

        // Save file
        bool save(std::string filename) {
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : Opening file \"%s\"...\n", 
                filename.data(), 
                filename.data());
#endif
            auto file = std::ofstream(filename, std::ios::out | std::ios::binary);
            if(!file) {
#ifdef LIBQIMG_SHOWLOG
                printf("[FMAT \"%s\" ] : Cannot open file \"%s\".\n", 
                    filename.data(), 
                    filename.data());
#endif
                return false;
            }
            // Write metadata
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : Writing metadata...\n", filename.data());
#endif
            file.write((char*)&FMAT_SIGNATURE, 4);
            file.write((char*)&size, 8);
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : Writing matrix data...\n", filename.data());
#endif
            for(int y = 0; y < size.y; y++)
                for(int x = 0; x < size.x; x++)
                    file.write((char*)&dataptr[y * size.x + x], 4);
            file.close();
#ifdef LIBQIMG_SHOWLOG
            printf("[FMAT \"%s\" ] : File \"%s\" wrote successfully.\n", 
                filename.data(), 
                filename.data());
#endif
            return true;
        }

        // Free memory
        void dispose() {
            if(dataptr != nullptr)
                delete[] dataptr;
        }

        // Adjust the canvas size.
        void resizeCanvas(int sizeX, int sizeY) {
            float* newDataptr = new float[sizeX * sizeY];
            for(int y = 0; y < size.y; y++)
                for(int x = 0; x < size.x; x++)
                    newDataptr[y * sizeX + x] = dataptr[y * size.x + x];
                    
            delete[] dataptr;
            dataptr = newDataptr;
        }

        inline Point canvasSize() const { return size; }
        inline int width() const { return size.x; }
        inline int height() const { return size.y; }

        inline float aspectRatio() const { return (float)size.x / (float)size.y; }

        // Return the Point mapped from [-0.5 ~ size - 0.5] to [-1, 1]
        inline PointF coordinate(PointF position) const{
            return PointF::reverseLerp(
                PointF(((float)size.x) / 2.0f, ((float)size.y) / 2.0f),
                PointF((float)size.x, (float)size.y),
                position);
        }

        // Return the Point mapped from [-0.5 ~ size - 0.5] to [-1, 1]
        inline PointF coordinate(Point position) const {
            return coordinate(PointF((float)position.x + 0.5f, (float)position.y + 0.5f));
        }

        // Return the pixel position of coordinate
        inline PointF positionFloat(PointF coordinate) const {
            return PointF::lerp(
                PointF(((float)(size.x)) / 2.0f, ((float)(size.y)) / 2.0f),
                PointF((float)(size.x), (float)(size.y)),
                coordinate);
        }
        
        // Return the pixel position of coordinate
        inline Point position(PointF coordinate) const {
            return Point(PointF::lerp(
                PointF(((float)(size.x)) / 2.0f, ((float)(size.y)) / 2.0f),
                PointF((float)(size.x), (float)(size.y)),
                coordinate - PointF(0.5f, 0.5f)));
        }

        // The left down corner point
        Point end() const { return Point(size.x - 1, size.y - 1); }

        inline float& operator()(int x, int y, TileMode::TileMode tileMode = TileMode::clamp) {

            if(x >= 0 && y >= 0 && x < size.x && y < size.y)
                return dataptr[size.x * y + x];
            
            switch (tileMode) {

                case TileMode::empty:
                    return _LIBQIMG_FMAT_SAFEADDRESS;
                    
                case TileMode::clamp: {
                    x = math::clamp(x, 0, size.x - 1);
                    y = math::clamp(y, 0, size.y - 1);
                    return dataptr[size.x * y + x];
                }

                case TileMode::mirror: {
                    x = math::mod(x, size.x);
                    y = math::mod(y, size.y);
                    return dataptr[size.x * y + x];
                }
                
                default: {
                    return _LIBQIMG_FMAT_SAFEADDRESS;
                }
            }
        }
        inline float& operator()(Point pt, TileMode::TileMode tileMode = TileMode::clamp) {
             return (*this)(pt.x, pt.y, tileMode);
        }
        inline float& operator()(PointF pt, TileMode::TileMode tileMode = TileMode::clamp) {
            return (*this)(static_cast<Point>(pt), tileMode); 
        }

        // Get value in readonly mode
        float pixelAccess(
            int x, int y, 
            TileMode::TileMode tileMode = TileMode::clamp, 
            float empty = 0.0
        ) const {

            if(x >= 0 && y >= 0 && x < size.x && y < size.y)
                return dataptr[size.x * y + x];
            
            switch (tileMode) {

                case TileMode::empty:
                    return _LIBQIMG_FMAT_SAFEADDRESS;
                    
                case TileMode::clamp: {
                    x = math::clamp(x, 0, size.x - 1);
                    y = math::clamp(y, 0, size.y - 1);
                    return dataptr[size.x * y + x];
                }

                case TileMode::mirror: {
                    x = math::mod(x, size.x);
                    y = math::mod(y, size.y);
                    return dataptr[size.x * y + x];
                }
                
                default: {
                    return _LIBQIMG_FMAT_SAFEADDRESS;
                }
            }
        }

        inline float sample(
            PointF pt, 
            SampleMode::SampleMode sampleMode = SampleMode::nearest,
            TileMode::TileMode tileMode = TileMode::clamp
        ) {
            pt -= PointF(0.5f, 0.5f);
            float lu = (*this)(floorf(pt.x), floorf(pt.y), tileMode);
            float ru = (*this)(ceilf(pt.x), floorf(pt.y), tileMode);
            float ld = (*this)(floorf(pt.x), ceilf(pt.y), tileMode);
            float rd = (*this)(ceilf(pt.x), ceilf(pt.y), tileMode);
            PointF sp = pt - PointF(floorf(pt.x), floorf(pt.y));
            if(sampleMode == SampleMode::nearest)
                return SampleMode::nearestSample(lu, ru, ld, rd, sp);
            else if(sampleMode == SampleMode::bilinear)
                return SampleMode::bilinearSample(lu, ru, ld, rd, sp);
            return 0.0f;
        }

        #define FMAT_FOREACH_PARAMS float& reference

        // Execute specific function from begin to end, function parameters: (float& reference)
        template <class Function>
        void foreach(Function function, Point begin, Point end) {
            for(int y = begin.y; y <= end.y; y++)
                for(int x = begin.x; x <= end.x; x++)
                    function((*this)(x, y));
        };

        /// Execute specific function from begin to end, function parameters: (float& reference)
        template <class Function>
        void foreach(Function function) {
            for(int y = 0; y < size.y; y++)
                for(int x = 0; x < size.x; x++)
                    function((*this)(x, y));
        };

        #define FMAT_PARAMETERIZED_FOREACH_PARAMS float& reference, Point current, FloatPointMatrix& matrix
        
        // Execute specific function from begin to end, function parameters: (float& reference, Point current, FloatPointMatrix* matrix)
        template <class Function>
        void parameterizedForeach(Function function, Point begin, Point end ) {
            for(int y = begin.y; y <= end.y; y++)
                for(int x = begin.x; x <= end.x; x++)
                    function((*this)(x, y), (Point){x, y}, *this);
        };

        // Execute specific function from begin to end, function parameters: (float& reference, Point current, FloatPointMatrix* matrix)
        template <class Function>
        void parameterizedForeach(Function function) {
            for(int y = 0; y < size.y; y++)
                for(int x = 0; x < size.x; x++)
                    function((*this)(x, y), (Point){x, y}, *this);
        };

        // Fill the matrix with a specific value
        void erase(const float& value = 0.0f) {
            foreach([value](float& reference) { reference = value; });
        }

        // Copy matrix size
        void copyCanvas(FloatPointMatrix& source) {
            delete[] dataptr;
            size = source.canvasSize();
            dataptr = new float[size.x, size.y];
        }

        // Copy content
        void copyContent(FloatPointMatrix& source) {
            for(int y = 0; y < size.y; y++)
                for(int x = 0; x < size.x; x++)
                    (*this)(x, y) = source(x, y);
        }

        // Copy matrix from source
        void copyFrom(FloatPointMatrix& source) {
            copyCanvas(source);
            copyContent(source);
        }

        // Create new object and copy from source
        static FloatPointMatrix copy(FloatPointMatrix& source) {
            FloatPointMatrix* ret = new FloatPointMatrix(source.canvasSize());
            ret->copyContent(source);
            return *ret;
        }

        // Return the average value of the appointed chunk
        float average(Point begin, Point end) {
            float sum = 0.0f;
            for(int y = begin.y; y <= end.y; y++)
                for(int x = begin.x; x <= end.x; x++)
                    sum += (*this)(x, y);
            return sum / (float)((end.x - begin.x + 1) * (end.y - begin.y + 1));
        }

        // Return the average value of the appointed chunk
        float average(PointF lu, PointF rd) {

            Point luOuter = Point(floorf(lu.x), floorf(lu.y));
            Point rdOuter = Point(ceilf(rd.x), ceilf(rd.y)) - Point(1, 1);

            float sum = 0.0f;

            for(int y = luOuter.y; y <= rdOuter.y; y++)
                for(int x = luOuter.x; x <= rdOuter.x; x++)
                    sum += (*this)(x, y);

            float lx = (lu.x - (float)luOuter.x), ly = (lu.y - (float)luOuter.y);
            float rx = ((float)rdOuter.x + 1.0f - rd.x), ry = ((float)rdOuter.y + 1.0f - rd.y);
            for(int y = luOuter.y; y <= rdOuter.y; y++) {
                sum -= (*this)(luOuter.x, y) * lx;
                sum -= (*this)(rdOuter.x, y) * rx;
            }
            for(int x = luOuter.x; x <= rdOuter.x; x++) {
                sum -= (*this)(x, luOuter.y) * ly;
                sum -= (*this)(x, rdOuter.y) * ry;
            }

            sum += (*this)(luOuter.x, luOuter.y) * lx * ly;
            sum += (*this)(luOuter.x, rdOuter.y) * lx * ry;
            sum += (*this)(rdOuter.x, luOuter.y) * rx * ly;
            sum += (*this)(rdOuter.x, rdOuter.y) * rx * ry;

            sum /= ((rd.x - lu.x) * (rd.y - lu.y));
            return sum;
        }
    };
    // Float Point Matrix
    typedef FloatPointMatrix FMAT;

}

#endif