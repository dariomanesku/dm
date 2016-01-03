/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_DATASTRUCTURES_COMMON_H_HEADER_GUARD
#define DM_DATASTRUCTURES_COMMON_H_HEADER_GUARD

#include <stdint.h>
#include "../common/common.h"               // DM_INLINE
#include "../../../3rdparty/bx/allocator.h" // dm::AllocatorI

namespace dm
{
    /// Heap alloc utils for dm data structures.
    ///
    /// Usage:
    ///     typedef dm::Array<int> IntArray;
    ///     IntArray* intArray = dm::create<IntArray>(64, &allocator);
    ///     /*...*/
    ///     dm::destroy(intArray);
    ///

    template <typename Ty>
    DM_INLINE Ty* create(uint32_t _max, void* _mem, dm::AllocatorI* _memDeallocator)
    {
        return ::new (_mem) Ty(_max, (uint8_t*)_mem + sizeof(Ty), _memDeallocator);
    }

    template <typename Ty>
    DM_INLINE Ty* create(uint32_t _max, dm::AllocatorI* _allocator)
    {
        uint8_t* ptr = (uint8_t*)BX_ALLOC(_allocator, sizeof(Ty) + Ty::sizeFor(_max));
        return create<Ty>(_max, ptr, _allocator);
    }

    template <typename Ty>
    DM_INLINE void destroy(Ty* _dataStructure)
    {
        _dataStructure->~Ty();
        BX_FREE(_dataStructure->allocator(), _dataStructure);
    }

} // namespace dm

#endif // DM_DATASTRUCTURES_COMMON_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
