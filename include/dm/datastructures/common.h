/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include "../allocatori.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_DATASTRUCTURES_COMMON_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_DATASTRUCTURES_COMMON_H_HEADER_GUARD
#   define DM_DATASTRUCTURES_COMMON_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    extern CrtAllocator g_crtAllocator;

    template <typename DataStructureH>
    DM_INLINE DataStructureH* create(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
    {
        uint8_t* ptr = (uint8_t*)DM_ALLOC(sizeof(DataStructureH) + DataStructureH::sizeFor(_max), _allocator);

        DataStructureH* dsb = ::new (ptr) DataStructureH();
        dsb->init(_max, ptr + sizeof(DataStructureH));
        dsb->m_allocator = _allocator;

        return dsb;
    }

    template <typename DataStructureH>
    DM_INLINE void destroy(DataStructureH* _dsb)
    {
        AllocatorI* allocator = _dsb->m_allocator;
        _dsb->~DataStructureH();
        DM_FREE(_allocator, _dsb);
    }
} // namespace DM_NAMESPACE
#   endif // DM_DATASTRUCTURES_COMMON_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
