/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_COMMON_HEADER_GUARD
#define DM_NG_COMMON_HEADER_GUARD

#include <dm/common/common.h> // DM_INLINE
#include <dm/ng/allocator.h>

namespace dm { namespace ng {

template <typename DataStructureH>
DM_INLINE DataStructureH* create(uint32_t _max, ReallocFn _reallocFn = ::realloc)
{
    uint8_t* ptr = (uint8_t*)dm_alloc(sizeof(DataStructureH) + DataStructureH::sizeFor(_max), _reallocFn);

    DataStructureH* dsb = ::new (ptr) DataStructureH();
    dsb->init(_max, ptr + sizeof(DataStructureH));
    dsb->m_reallocFn = _reallocFn;

    return dsb;
}

template <typename DataStructureH>
DM_INLINE void destroy(DataStructureH* _dsb)
{
    ReallocFn _reallocFn = _dsb->m_reallocFn;
    _dsb->~DataStructureH();
    dm_free(_dsb, _reallocFn);
}

} //namespace ng
} //namespace dm

#endif // DM_NG_COMMON_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
