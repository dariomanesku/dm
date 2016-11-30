/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
#   include "platform.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_DEBUG_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_DEBUG_H_HEADER_GUARD
#   define DM_DEBUG_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    DM_INLINE void debugBreak()
    {
    #if DM_COMPILER_MSVC
        __debugbreak();
    #elif (DM_COMPILER_GCC || DM_COMPILER_CLANG) && DM_X86
        __asm__("int $3");
    #else // Cross platform implementation.
        int* int3 = (int*)3L;
        *int3 = 3;
    #endif
    }
} // namespace DM_NAMESPACE
#   endif // DM_DEBUG_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
