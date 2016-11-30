/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include "platform.h"
    #include <stdint.h>
    #if DM_PLATFORM_WINDOWS
    #   include <windows.h>
    #else
    #   include <sys/time.h> // gettimeofday
    #endif // DM_PLATFORM_
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_TIMER_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_TIMER_H_HEADER_GUARD
#   define DM_TIMER_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    /*
     * Adapted from: https://github.com/bkaradzic/bx/include/bx/timer.h
     * Copyright 2010-2016 Branimir Karadzic. All rights reserved.
     * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
     */

    inline int64_t getHPCounter()
    {
        #if DM_PLATFORM_WINDOWS
            LARGE_INTEGER li;
            // Performance counter value may unexpectedly leap forward
            // http://support.microsoft.com/kb/274323
            QueryPerformanceCounter(&li);
            int64_t i64 = li.QuadPart;
        #else
            struct timeval now;
            gettimeofday(&now, 0);
            int64_t i64 = now.tv_sec*INT64_C(1000000) + now.tv_usec;
        #endif // DM_PLATFORM_

        return i64;
    }

    inline int64_t getHPFrequency()
    {
        #if DM_PLATFORM_WINDOWS
            LARGE_INTEGER li;
            QueryPerformanceFrequency(&li);
            return li.QuadPart;
        #else
            return INT64_C(1000000);
        #endif // DM_PLATFORM_
    }
} // namespace DM_NAMESPACE
#   endif // DM_TIMER_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
