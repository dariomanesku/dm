/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_DEBUG_HEADER_GUARD
#define DM_NG_DEBUG_HEADER_GUARD

namespace dm { namespace ng {

inline void debugBreak()
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

} //namespace ng
} //namespace dm

#endif //DM_NG_DEBUG_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
