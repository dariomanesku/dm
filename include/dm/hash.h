/*
 * Copyright 2014-2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
#   include <stdint.h>
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_HASH_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_HASH_H_HEADER_GUARD
#   define DM_HASH_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    DM_INLINE uint32_t hashStr(const char _str[]) // Null terminated string.
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
        return DM_NAMESPACE::hash(reinterpret_cast<const uint8_t*>(&_val), sizeof(Ty));
    }

} // namespace DM_NAMESPACE
#   endif // DM_HASH_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
