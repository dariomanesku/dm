/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <stdint.h>
    #include "platform.h"

    #if DM_COMPILER_MSVC
    #   include <intrin.h>
    #   pragma intrinsic(_BitScanForward)
    #   pragma intrinsic(_BitScanReverse)
    #   if DM_ARCH_64BIT
    #       pragma intrinsic(_BitScanForward64)
    #       pragma intrinsic(_BitScanReverse64)
    #   endif // DM_ARCH_64BIT
    #endif // DM_COMPILER_MSVC
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_BITOPS_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_BITOPS_HEADER_GUARD
#   define DM_BITOPS_HEADER_GUARD
namespace DM_NAMESPACE
{
    /// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
    DM_INLINE uint32_t cntbits_u32_ref(uint32_t _val)
    {
        _val = _val - ((_val >> 1) & 0x55555555);
        _val = (_val & 0x33333333) + ((_val >> 2) & 0x33333333);
        uint32_t cnt = ((_val + ((_val >> 4) & 0xf0f0f0f)) * 0x1010101) >> 24;

        return cnt;
    }

    DM_INLINE uint32_t cntlz_u32_ref(uint32_t _val)
    {
        _val |= _val >> 1;
        _val |= _val >> 2;
        _val |= _val >> 4;
        _val |= _val >> 8;
        _val |= _val >> 16;
        return cntbits_u32_ref(~_val);
    }

    DM_INLINE uint32_t cnttz_u32_ref(uint32_t _val)
    {
        uint32_t to = (~_val)&(_val-1);
        return cntbits_u32_ref(to);
    }

    DM_INLINE uint64_t cntbits_u64_ref(uint64_t _val)
    {
        uint32_t lo = uint32_t(_val&UINT32_MAX);
        uint32_t hi = uint32_t(_val>>32);

        uint32_t total = cntbits_u32_ref(lo)
                       + cntbits_u32_ref(hi);

        return total;
    }

    DM_INLINE uint32_t cntlz_u32(uint32_t _val)
    {
    #if DM_COMPILER_GCC || DM_COMPILER_CLANG
        return __builtin_clz(_val);
    #elif DM_COMPILER_MSVC && DM_PLATFORM_WINDOWS
        unsigned long idx;
        _BitScanReverse(&idx, _val);
        return 31 - idx;
    #else
        return cntlz_u32_ref(_val);
    #endif // DM_COMPILER_
    }

    DM_INLINE uint32_t cnttz_u32(uint32_t _val)
    {
    #if DM_COMPILER_GCC || DM_COMPILER_CLANG
        return __builtin_ctz(_val);
    #elif DM_COMPILER_MSVC && DM_PLATFORM_WINDOWS
        unsigned long idx;
        _BitScanForward(&idx, _val);
        return idx;
    #else
        return cnttz_u32_ref(_val);
    #endif // DM_COMPILER_
    }

    DM_INLINE uint32_t cntlz_u64_ref(uint64_t _val)
    {
        return (_val & UINT64_C(0xffffffff00000000))
             ? cntlz_u32(uint32_t(_val>>32))
             : cntlz_u32(uint32_t(_val)) + 32
             ;
    }

    DM_INLINE uint32_t cnttz_u64_ref(uint64_t _val)
    {
        return (_val & UINT64_C(0xffffffff))
            ? cnttz_u32(uint32_t(_val))
            : cnttz_u32(uint32_t(_val>>32)) + 32
            ;
    }

    DM_INLINE uint32_t cntbits_u32(uint32_t _val)
    {
    #if DM_COMPILER_GCC || DM_COMPILER_CLANG
        return __builtin_popcount(_val);
    #elif DM_COMPILER_MSVC && DM_PLATFORM_WINDOWS
        return __popcnt(_val);
    #else
        return cntbits_u32_ref(_val);
    #endif // DM_COMPILER_
    }

    DM_INLINE uint64_t cntbits_u64(uint64_t _val)
    {
    #if DM_COMPILER_GCC || DM_COMPILER_CLANG
        return __builtin_popcountll(_val);
    #elif DM_COMPILER_MSVC && DM_ARCH_64BIT
        return __popcnt64(_val);
    #else
        return cntbits_u64_ref(_val);
    #endif // DM_COMPILER_
    }

    DM_INLINE uint64_t cntlz_u64(uint64_t _val)
    {
    #if DM_COMPILER_GCC || DM_COMPILER_CLANG
        return __builtin_clzll(_val);
    #elif DM_COMPILER_MSVC && DM_PLATFORM_WINDOWS && DM_ARCH_64BIT
        unsigned long idx;
        _BitScanReverse64(&idx, _val);
        return 63 - idx;
    #else
        return cntlz_u64_ref(_val);
    #endif // DM_COMPILER_
    }

    DM_INLINE uint64_t cnttz_u64(uint64_t _val)
    {
    #if DM_COMPILER_GCC || DM_COMPILER_CLANG
        return __builtin_ctzll(_val);
    #elif DM_COMPILER_MSVC && DM_PLATFORM_WINDOWS && DM_ARCH_64BIT
        unsigned long idx;
        _BitScanForward64(&idx, _val);
        return idx;
    #else
        return cnttz_u64_ref(_val);
    #endif // DM_COMPILER_
    }

} // namespace DM_NAMESPACE
#   endif // DM_BITOPS_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
