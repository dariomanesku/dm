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
#include "../compiletime.h"   // dm::bestfit_type<>::type, TyInfo<>::Max()

#include "../../../3rdparty/bx/allocator.h" // dm::ReallocatorI

namespace dm
{
    template <uint32_t MaxHandlesT>
    struct HandleAllocT
    {
        typedef typename bestfit_type<MaxHandlesT>::type HandleType;

        HandleAllocT()
        {
            reset();
        }

        #include "handlealloc_inline_impl.h"

        HandleType count() const
        {
            return m_numHandles;
        }

        HandleType max() const
        {
            return MaxHandlesT;
        }

    private:
        HandleType m_handles[MaxHandlesT];
        HandleType m_indices[MaxHandlesT];
        HandleType m_numHandles;
    };

    template <typename HandleTy=uint16_t>
    struct HandleAlloc
    {
        typedef HandleTy HandleType;

        // Uninitialized state, init() needs to be called !
        HandleAlloc()
        {
            m_handles = NULL;
            m_indices = NULL;
        }

        HandleAlloc(HandleType _max, dm::ReallocatorI* _reallocator)
        {
            init(_max, _reallocator);
        }

        HandleAlloc(HandleType _max, void* _mem, dm::AllocatorI* _allocator)
        {
            init(_max, _mem, _allocator);
        }

        ~HandleAlloc()
        {
            destroy();
        }

        // Allocates memory internally.
        void init(HandleType _max, dm::ReallocatorI* _reallocator)
        {
            m_maxHandles = _max;
            m_handles = (HandleType*)DM_ALLOC(_reallocator, sizeFor(_max));
            m_indices = m_handles + _max;
            m_reallocator = _reallocator;
            m_cleanup = true;

            reset();
        }

        enum
        {
            SizePerElement = 2*sizeof(HandleType),
        };

        static inline uint32_t sizeFor(HandleType _max)
        {
            return _max*SizePerElement;
        }

        // Uses externally allocated memory.
        void* init(HandleType _max, void* _mem, dm::AllocatorI* _allocator = NULL)
        {
            m_maxHandles = _max;
            m_handles = (HandleType*)_mem;
            m_indices = m_handles + _max;
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
                DM_FREE(m_reallocator, m_handles);
                m_handles = NULL;
                m_indices = NULL;
            }

            m_numHandles = 0;
        }

        #include "handlealloc_inline_impl.h"

        HandleType count() const
        {
            return m_numHandles;
        }

        HandleType max() const
        {
            return m_maxHandles;
        }

        dm::AllocatorI* allocator()
        {
            return m_allocator;
        }

    private:
        HandleType m_numHandles;
        HandleType m_maxHandles;
        HandleType* m_handles;
        HandleType* m_indices;
        union
        {
            dm::AllocatorI*   m_allocator;
            dm::ReallocatorI* m_reallocator;
        };
        bool m_cleanup;
    };
    typedef HandleAlloc<uint8_t>  HandleAlloc8;
    typedef HandleAlloc<uint16_t> HandleAlloc16;
    typedef HandleAlloc<uint32_t> HandleAlloc32;

} // namespace dm

#endif // DM_HANDLEALLOC_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
