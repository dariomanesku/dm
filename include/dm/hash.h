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
    DM_INLINE uint32_t hash(const char _str[]) // Null terminated string.
    {
        // Sdbm hash from public domain.

        uint32_t hash = 0;
        for (const char* ch = _str; *ch != '\0'; ++ch)
        {
            hash = uint32_t(*ch) + (hash << 6) + (hash << 16) - hash;
        }

        return hash;
    }

    DM_INLINE uint32_t hash(const void* _data, uint32_t _size)
    {
        // Sdbm hash from public domain.

        uint32_t hash = 0;
        for (uint32_t ii = _size; ii--; )
        {
            const uint8_t* bytes = (const uint8_t*)_data;
            hash = uint32_t(bytes[ii]) + (hash << 6) + (hash << 16) - hash;
        }

        return hash;
    }

    template <typename Ty>
    DM_INLINE uint32_t hash(const Ty& _val)
    {
        return dm::hash(reinterpret_cast<const void*>(&_val), sizeof(_val));
    }

} // namespace dm

#endif // DM_HASH_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
