
//  Copyright 2021 Isoheptane
//  Filename    : fmat.hpp
//  Purpose     : Type of FloatPointMatrix Collection
//  License     : MIT License

#ifndef _LIBQIMG_FMC_HPP_
#define _LIBQIMG_FMC_HPP_

#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>

#include "libqimg_debuglog.hpp"
#include "point.hpp"
#include "fmat.hpp"

namespace libqimg {

    /*
        Float Point Matrix Collection File Structure (.fmc)
        0x0000  int32   Signature 80 72 7F A5
        0x0004  int32   Width
        0x0008  int32   Height
        0x000B  uint16  ChannelCount
        [
            byte    Tag Length
            string  Channel Tag
            float   Data
        ]

        In the end of this file, you can write anything you want.
        Program won't read the tail of the file.

    */

    const int FMC_SIGNATURE = 0x80797FA5;
    // Float Point Matrix Collection
    class FloatPointMatrixCollection {
      private:
        bool openSucceed = false;
        Point size;
        unsigned short channelCount;
        std::string* channelTags;
        FloatPointMatrix** channels;
      public:

        // Initialize a collection by size and channel count.
        FloatPointMatrixCollection(
            Point size, 
            unsigned short channelCount
        ):size(size), channelCount(channelCount) {
            
            channels = new FloatPointMatrix*[channelCount];
            channelTags = new std::string[channelCount];
            for(int i = 0; i < channelCount; i++)
                channels[i] = new FloatPointMatrix(size);
            openSucceed = true;
        }

        // Initialize a collection by size and channel count.
        FloatPointMatrixCollection(
            int sizeX, int sizeY, 
            unsigned short channelCount
        ):size(Point(sizeX, sizeY)), channelCount(channelCount) {

            channels = new FloatPointMatrix*[channelCount];
            channelTags = new std::string[channelCount];
            for(int i = 0; i < channelCount; i++)
                channels[i] = new FloatPointMatrix(size);
            openSucceed = true;
        }

        // Read collection from file
        FloatPointMatrixCollection(std::string filename) {
            
            char tagTemp[256];
#ifdef LIBQIMG_SHOWLOG
            printf("[FMC \"%s\" ] Opening file \"%s\" ...\n", filename.data(), filename.data());
#endif    
            auto file = std::ifstream(filename, std::ios::in | std::ios::binary);
            if(!file) {
#ifdef LIBQIMG_SHOWLOG
            printf("[FMC \"%s\" ] Cannot open file \"%s\" .\n", filename.data(), filename.data());
#endif
                return;
            }
            // Check file signature
            int fileSignature;
            file.read((char*)&fileSignature, 4);
            if(fileSignature != FMC_SIGNATURE) {
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ] \"%s\"  signature incorrect.\n", 
                    filename.data(), 
                    filename.data());
#endif
                return;
            }
#ifdef LIBQIMG_SHOWLOG
            printf("[FMC \"%s\" ] Reading metadata...\n", filename.data());
#endif
            file.read((char*)&size, 8);
            file.read((char*)&channelCount, 2);
            // Build channels and readin
            channels = new FloatPointMatrix*[channelCount];
            channelTags = new std::string[channelCount];
#ifdef LIBQIMG_SHOWLOG
            printf("[FMC \"%s\" ]  -> Resolution: %dx%d\n",
                filename.data(),
                size.x,
                size.y);
            printf("[FMC \"%s\" ]  -> Channel Count: %d channel(s)\n",
                filename.data(),
                channelCount);
#endif
            for(int ch = 0; ch < channelCount; ch++) {
                // Allocate memory
                channels[ch] = new FloatPointMatrix(size);
                // Read channel tag
                unsigned char tagLen;
                file.read((char*)&tagLen, 1);
                file.read(tagTemp, tagLen);
                tagTemp[tagLen] = '\0';
                channelTags[ch] = std::string(tagTemp);
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ] Channel %d: \"%s\" \n", 
                    filename.data(), 
                    ch, 
                    channelTags[ch].data());
#endif
                // Read Matrix Data
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ]  -> Reading channel %d matrix data...\n", 
                    filename.data(), ch);
#endif
                for(int y = 0; y < size.y; y++)
                    for(int x = 0; x < size.x; x++)
                        file.read((char*)&(*channels[ch])(x, y), 4);
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ]  -> Channel %d matrix loaded.\n", 
                    filename.data(), ch);
#endif
            }
            file.close();
#ifdef LIBQIMG_SHOWLOG
            printf("[FMC \"%s\" ] File \"%s\"  loaded successfully.\n", 
                filename.data(), 
                filename.data());
#endif
            openSucceed = true;
        }

        // Save file
        bool save(std::string filename) {
            
#ifdef LIBQIMG_SHOWLOG 
            printf("[FMC \"%s\" ] Opening file \"%s\" ...\n", 
                filename.data(), 
                filename.data());
#endif
            auto file = std::ofstream(filename, std::ios::out | std::ios::binary);
            if(!file) {
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ] Cannot open file \"%s\" .\n", 
                    filename.data(), 
                    filename.data());
#endif
                return false;
            }
            // Write metadata
#ifdef LIBQIMG_SHOWLOG
            printf("[FMC \"%s\" ] Writing metadata...\n", filename.data());
#endif
            file.write((char*)&FMC_SIGNATURE, 4);
            file.write((char*)&size, 8);
            file.write((char*)&channelCount, 2);
            for(int ch = 0; ch < channelCount; ch++) {
                // Write Channel Tag
                unsigned char tagLen = channelTags[ch].length();
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ] Channel %d: \"%s\" \n", 
                    filename.data(), 
                    ch, 
                    channelTags[ch].data());
#endif
                file.write((char*)&tagLen, 1);
                file.write(channelTags[ch].data(), tagLen);
                // Write Matrix Data
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ]  -> Writing channel %d matrix data...\n", 
                    filename.data(), 
                    ch);
#endif
                for(int y = 0; y < size.y; y++)
                    for(int x = 0; x < size.x; x++)
                        file.write((char*)&(*channels[ch])(x, y), 4);
#ifdef LIBQIMG_SHOWLOG
                printf("[FMC \"%s\" ]  -> Channel %d matrix wrote.\n", 
                    filename.data(), 
                    ch);
#endif
            }
            file.close();
#ifdef LIBQIMG_SHOWLOG
            printf("[FMC \"%s\" ] File \"%s\"  wrote successfully.\n", 
                filename.data(), 
                filename.data());
#endif
            return true;
        }


        // Free memory
        void dispose() {
            for(int i = 0; i < channelCount; i++)
                channels[i]->dispose();
            delete[] channels;
            delete[] channelTags;
        }

        // Adjust the canvas size.
        void resizeCanvas(int sizeX, int sizeY) {
            for(int i = 0; i < channelCount; i++)
                channels[i]->resizeCanvas(sizeX, sizeY);
        }

        /* Function same as FMAT */
        inline Point canvasSize() const { return size; }
        inline int width() const { return size.x; }
        inline int height() const { return size.y; }
        // Return the channel count;
        inline unsigned short count() const { return channelCount; }
        inline std::string& channelName(unsigned short channelID) { return channelTags[channelID]; }

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

        inline FloatPointMatrix& operator[](int index) { return (*channels[index]); }
        
        FloatPointMatrix& operator[](std::string index) {
            for(int ch = 0; ch < channelCount; ch++)
                if(index == channelTags[ch])
                    return (*channels[ch]);
            return *(new FloatPointMatrix(0, 0));
        }

        // Return the channel ID
        int indexOf(std::string index) {
            for(int ch = 0; ch < channelCount; ch++)
                if(index == channelTags[ch])
                    return ch;
            return -1;
        }
        

        #define FMC_CANVAS_FOREACH_PARAMS const Point& current, FloatPointMatrixCollection& collection

        // Execute specific function at all position from begin to end, function parameters: (Point current, FloatPointMatrixCollection& collection, FloatPointMatrix& matrix)
        template <class Function>
        void canvasForeach(Function function, Point begin, Point end) {
            for(int y = begin.y; y <= end.y; y++)
                for(int x = begin.x; x <= end.x; x++)
                    function(Point(x, y), *this);
        };

        // Execute specific function at all position from begin to end, function parameters: (Point current, FloatPointMatrixCollection& collection, FloatPointMatrix& matrix)
        template <class Function>
        void canvasForeach(Function function) {
            for(int y = 0; y < size.y; y++)
                for(int x = 0; x < size.x; x++)
                    function(Point(x, y), *this);
        };

        #define FMC_CHANNEL_FOREACH_PARAMS const Point& current, unsigned short channelID, FloatPointMatrixCollection& collection, FloatPointMatrix& matrix

        // Execute specific function at all position from begin to end, function parameters: (Point current, unsigned short channelID, FloatPointMatrixCollection& collection, FloatPointMatrix& matrix)
        template <class Function>
        void channelForeach(Function function, Point begin, Point end) {
            for(int ch = 0; ch < channelCount; ch++)
                for(int y = begin.y; y <= end.y; y++)
                    for(int x = begin.x; x <= end.x; x++)
                        function(Point(x, y), ch, *this, (*this)[ch]);
        };

        // Execute specific function at all position from begin to end, function parameters: (Point current, unsigned short channelID, FloatPointMatrixCollection& collection, FloatPointMatrix& matrix)
        template <class Function>
        void channelForeach(Function function) {
            for(int ch = 0; ch < channelCount; ch++)
                for(int y = 0; y < size.y; y++)
                    for(int x = 0; x < size.x; x++)
                        function(Point(x, y), ch, *this, (*this)[ch]);
        };

        // Copy matrix size
        void copyCanvas(FloatPointMatrixCollection& source) {
            dispose();
            size = source.canvasSize();
            channelCount = source.count();
            channelTags = new std::string[channelCount];
            channels = new FloatPointMatrix*[channelCount];
            for(int ch = 0; ch < channelCount; ch++) {
                channels[ch] = new FloatPointMatrix(size);
                channelTags[ch] = source.channelName(ch);
            }
        }
        
        // Copy content
        void copyContent(FloatPointMatrixCollection& source) {
            for(int ch = 0; ch < channelCount; ch++)
                for(int y = 0; y < size.y; y++)
                    for(int x = 0; x < size.x; x++)
                        (*this)[ch](x, y) = source[ch](x, y);
        }

        // Copy collection from source
        void copyFrom(FloatPointMatrixCollection& source) {
            copyCanvas(source);
            copyContent(source);
        }

        // Create new object and copy from source
        static FloatPointMatrixCollection copy(FloatPointMatrixCollection& source) {
            FloatPointMatrixCollection* ret = new FloatPointMatrixCollection(source.canvasSize(), source.count());
            ret->copyContent(source);
            return *ret;
        }

    };
    // Float Point Matrix Collection
    typedef FloatPointMatrixCollection FMC;

}

#endif