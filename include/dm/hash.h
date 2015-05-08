/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_HASH_H_HEADER_GUARD
#define DM_HASH_H_HEADER_GUARD

#include <stdint.h>
#include "common/common.h" // DM_INLINE()

namespace dm
{
    DM_INLINE uint32_t hash(const char _str[])
    {
        // Sdbm hash from public domain.

        uint32_t hash = 0;
        for (const char* ch = _str; ch; ++ch)
        {
            hash = uint32_t(*ch) + (hash << 6) + (hash << 16) - hash;
        }

        return hash;
    }

    DM_INLINE uint32_t hash(const uint8_t* _bytes, uint32_t _size)
    {
        // Sdbm hash from public domain.

        uint32_t hash = 0;
        for (uint32_t ii = _size; ii; --ii)
        {
            hash = uint32_t(_bytes[ii]) + (hash << 6) + (hash << 16) - hash;
        }

        return hash;
    }

    DM_INLINE uint32_t hash(const char* _str, uint32_t _len)
    {
        return dm::hash((const uint8_t*)_str, _len);
    }

    template <typename Ty>
    DM_INLINE uint32_t hash(const Ty& _val)
    {
        return dm::hash(reinterpret_cast<const uint8_t*>(&_val), sizeof(_val));
    }

} // namespace dm

#endif // DM_HASH_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
