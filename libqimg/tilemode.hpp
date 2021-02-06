
//  Copyright 2021 Isoheptane
//  Filename    : tilemode.hpp
//  Purpose     : Support Basic Tile Mode selection
//  License     : MIT License

#ifndef _LIBQIMG_TILEMODE_HPP_
#define _LIBQIMG_TILEMODE_HPP_

#include "libqimg_math.hpp"

namespace libqimg::TileMode {
    
    // Tile Mode selection
    enum TileMode {
        empty = 0,
        clamp = 1,
        repeat = 2,
        mirror = 3
    };

}

#endif