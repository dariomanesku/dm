/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_LIST_H_HEADER_GUARD
#define DM_LIST_H_HEADER_GUARD

#include <stdint.h> // uint32_t
#include <new>      // placement-new

#include "common.h" // Heap alloc utils.

#include "../common/common.h" // DM_INLINE
#include "../check.h"         // DM_CHECK

#include "../../../3rdparty/bx/allocator.h" // bx::ReallocatorI

#include "handlealloc.h"

namespace dm
{
    template <typename ObjTy, uint32_t MaxT>
    struct ListT
    {
        typedef ListT<ObjTy,MaxT> This;
        #include "list_inline_impl.h"

        uint16_t count() const
        {
            return m_handles.count();
        }

        uint16_t max() const
        {
            return MaxT;
        }

    private:
        HandleAllocT<MaxT> m_handles;
        ObjTy m_elements[MaxT];
    };

    template <typename ObjTy/*obj type*/>
    struct List
    {
        // Uninitialized state, init() needs to be called !
        List()
        {
            m_memoryBlock = NULL;
        }

        List(uint16_t _max, bx::ReallocatorI* _reallocator)
        {
            init(_max, _reallocator);
        }

        List(uint16_t _max, void* _mem, bx::AllocatorI* _allocator)
        {
            init(_max, _mem, _allocator);
        }

        ~List()
        {
            destroy();
        }

        enum
        {
            SizePerElement = sizeof(ObjTy) + HandleAlloc16::SizePerElement,
        };

        static inline uint32_t sizeFor(uint16_t _max)
        {
            return _max*SizePerElement;
        }

        // Allocates memory internally.
        void init(uint16_t _max, bx::ReallocatorI* _reallocator)
        {
            m_memoryBlock = BX_ALLOC(_reallocator, sizeFor(_max));
            m_reallocator = _reallocator;
            m_cleanup = true;

            void* ptr = m_handles.init(_max, m_memoryBlock);
            m_elements = (ObjTy*)ptr;
        }

        // Uses externally allocated memory.
        void* init(uint16_t _max, void* _mem, bx::AllocatorI* _allocator = NULL)
        {
            m_memoryBlock = _mem;
            m_allocator = _allocator;
            m_cleanup = false;

            void* ptr = m_handles.init(_max, m_memoryBlock);
            m_elements = (ObjTy*)ptr;

            void* end = (void*)((uint8_t*)_mem + sizeFor(_max));
            return end;
        }

        bool isInitialized() const
        {
            return (NULL != m_memoryBlock);
        }

        void destroy()
        {
            if (NULL != m_memoryBlock)
            {
                m_handles.destroy();
                if (m_cleanup)
                {
                    BX_FREE(m_reallocator, m_memoryBlock);
                }
                m_memoryBlock = NULL;
            }
        }

        typedef List<ObjTy> This;
        #include "list_inline_impl.h"

        uint16_t count() const
        {
            return m_handles.count();
        }

        uint16_t max() const
        {
            return m_handles.max();
        }

        bx::AllocatorI* allocator()
        {
            return m_allocator;
        }

    private:
        HandleAlloc16 m_handles;
        ObjTy* m_elements;
        void* m_memoryBlock;
        union
        {
            bx::AllocatorI*   m_allocator;
            bx::ReallocatorI* m_reallocator;
        };
        bool m_cleanup;
    };

} // namespace dm

#endif // DM_LIST_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
