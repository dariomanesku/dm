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
DM_INLINE DataStructureH* create(uint32_t _max, Allocator* _allocator)
{
    uint8_t* ptr = (uint8_t*)_allocator->m_allocFunc(sizeof(DataStructureH) + DataStructureH::sizeFor(_max));

    DataStructureH* dsb = ::new (ptr) DataStructureH();
    dsb->init(_max, ptr + sizeof(DataStructureH));
    dsb->m_freeFunc = _allocator->m_freeFunc;

    return dsb;
}

template <typename DataStructureH>
DM_INLINE void destroy(DataStructureH* _dsb)
{
    FreeFunc freeFunc = _dsb->m_freeFunc;
    _dsb->~DataStructureH();
    freeFunc(_dsb);
}

} //namespace ng
} //namespace dm

#endif // DM_NG_COMMON_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
