/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_PI_H_HEADER_GUARD
#define DM_PI_H_HEADER_GUARD

#include "common/common.h" // DM_INLINE()

namespace dm
{
    static const float pi        = 3.14159265358979323846f;
    static const float twoPi     = 6.28318530717958647692f;
    static const float piHalf    = 1.57079632679489661923f;
    static const float invPi     = 0.31830988618379067153f;
    static const float invPiHalf = 0.15915494309189533576f;

    static const float toRad     = 0.01745329251994329576f; // pi/180.0
    static const float toDeg     = 57.2957795130823208767f; // 180.0/pi

    DM_INLINE float degToRad(float _deg)
    {
        return _deg*toRad;
    }

    DM_INLINE float radToDeg(float _rad)
    {
        return _rad*toDeg;
    }

} // namespace dm

#endif // DM_PI_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
