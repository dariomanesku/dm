/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_HANDLEALLOC_H_HEADER_GUARD
#define DM_HANDLEALLOC_H_HEADER_GUARD

#include <stdint.h> // uint32_t
#include <new> // placement-new

#include "common.h" // Heap alloc utils.

#include "../common/common.h" // DM_INLINE
#include "../check.h"         // DM_CHECK

#include "../../../3rdparty/bx/allocator.h" // bx::ReallocatorI

namespace dm
{
    // Adapted from: https://github.com/bkaradzic/bx/blob/master/include/bx/handlealloc.h

    template <uint32_t MaxHandlesT, typename HandleTy=uint16_t/*uint16_t or uint32_t*/>
    struct HandleAllocT
    {
        HandleAllocT()
        {
            reset();
        }

        #include "handlealloc_inline_impl.h"

        HandleTy count() const
        {
            return m_numHandles;
        }

        HandleTy max() const
        {
            return MaxHandlesT;
        }

    private:
        HandleTy m_handles[MaxHandlesT*2];
        HandleTy m_numHandles;
    };
    template <uint32_t MaxHandlesT> struct HandleAllocT16 : HandleAllocT<MaxHandlesT, uint16_t> { };
    template <uint32_t MaxHandlesT> struct HandleAllocT32 : HandleAllocT<MaxHandlesT, uint32_t> { };

    template <typename HandleTy=uint16_t /*uint16_t or uint32_t*/>
    struct HandleAlloc
    {
        // Uninitialized state, init() needs to be called !
        HandleAlloc()
        {
            m_handles = NULL;
        }

        HandleAlloc(HandleTy _max, bx::ReallocatorI* _reallocator)
        {
            init(_max, _reallocator);
        }

        HandleAlloc(HandleTy _max, void* _mem, bx::AllocatorI* _allocator)
        {
            init(_max, _mem, _allocator);
        }

        ~HandleAlloc()
        {
            destroy();
        }

        // Allocates memory internally.
        void init(HandleTy _max, bx::ReallocatorI* _reallocator)
        {
            m_maxHandles = _max;
            m_handles = (HandleTy*)BX_ALLOC(_reallocator, sizeFor(_max));
            m_reallocator = _reallocator;
            m_cleanup = true;

            reset();
        }

        enum
        {
            SizePerElement = 2*sizeof(HandleTy),
        };

        static inline uint32_t sizeFor(HandleTy _max)
        {
            return _max*SizePerElement;
        }

        // Uses externally allocated memory.
        void* init(HandleTy _max, void* _mem, bx::AllocatorI* _allocator = NULL)
        {
            m_maxHandles = _max;
            m_handles = (HandleTy*)_mem;
            m_allocator = _allocator;
            m_cleanup = false;

            reset();

            void* end = (void*)((uint8_t*)_mem + sizeFor(_max));
            return end;
        }

        bool isInitialized() const
        {
            return (NULL != m_handles);
        }

        void destroy()
        {
            if (m_cleanup && NULL != m_handles)
            {
                BX_FREE(m_reallocator, m_handles);
                m_handles = NULL;
            }

            m_numHandles = 0;
        }

        #include "handlealloc_inline_impl.h"

        HandleTy count() const
        {
            return m_numHandles;
        }

        HandleTy max() const
        {
            return m_maxHandles;
        }

        bx::AllocatorI* allocator()
        {
            return m_allocator;
        }

    private:
        HandleTy m_numHandles;
        HandleTy m_maxHandles;
        HandleTy* m_handles;
        union
        {
            bx::AllocatorI*   m_allocator;
            bx::ReallocatorI* m_reallocator;
        };
        bool m_cleanup;
    };
    typedef HandleAlloc<uint16_t> HandleAlloc16;
    typedef HandleAlloc<uint32_t> HandleAlloc32;

} // namespace dm

#endif // DM_HANDLEALLOC_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
