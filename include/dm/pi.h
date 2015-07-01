/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_PI_H_HEADER_GUARD
#define DM_PI_H_HEADER_GUARD

namespace dm
{
    static const float pi        = 3.14159265358979323846f;
    static const float twoPi     = 6.28318530717958647692f;
    static const float piHalf    = 1.57079632679489661923f;
    static const float invPi     = 0.31830988618379067153f;
    static const float invPiHalf = 0.15915494309189533576f;
    static const float toRad     = 0.0174532925199432957692369076848861271344287188854172f; // pi/180.0
    static const float toDeg     = 57.295779513082320876798154814105170332405472466564321f; // 180.0/pi

    DM_INLINE float degToRad(float _deg)
    {
        return _deg*(pi/180.0f);
    }

    DM_INLINE float radToDeg(float _rad)
    {
        return _rad*(180.0f/pi);
    }

} // namespace dm

#endif // DM_PI_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */

