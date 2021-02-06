
//  Copyright 2021 Isoheptane
//  Filename    : libqimg_math.hpp
//  Purpose     : Support Basic Mathematical Operations
//  License     : MIT License

#ifndef _LIBQIMG_MATH_HPP_
#define _LIBQIMG_MATH_HPP_

namespace libqimg::math {
    
    // Return the bigger value
    template <typename type> inline type max(type a, type b) { return a > b ? a : b; }
    // Return the smaller value
    template <typename type> inline type min(type a, type b) { return a > b ? b : a; }
    // Linear Interportation
    template <typename type> inline type lerp(type begin, type end, type t) {
        return (end - begin) * t + begin;
    }
    template <typename type> type inline reverseLerp(type begin, type end, type value) {
        return (value - begin) / (end - begin);
    }
    // Limit the value to a range from min_t to max_t
    template <typename type> type inline clamp(type t, type min_t, type max_t) {
        if(t < min_t) return min_t;
        else if(t > max_t) return max_t;
        else return t;
    }
    // Mod function that supports negative x
    inline int mod(int x, int mod) {
        if(x >= 0) return x % mod;
        else return (x - ((1 + x / mod) * mod)) % mod;
    }

}

#endif