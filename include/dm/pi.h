/*
 * Copyright 2014-2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_PI_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_PI_H_HEADER_GUARD
#   define DM_PI_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    #define DM_PI       3.14159265358979323846f
    #define DM_RPI      0.31830988618379067153f
    #define DM_2PI      6.28318530717958647692f
    #define DM_DEGTORAD 0.01745329251994329576f
    #define DM_RADTODEG 57.2957795130823208767f

    DM_INLINE float degToRad(float _deg) { return _deg*DM_DEGTORAD; }
    DM_INLINE float radToDeg(float _rad) { return _rad*DM_RADTODEG; }
} // namespace DM_NAMESPACE
#   endif // DM_PI_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
