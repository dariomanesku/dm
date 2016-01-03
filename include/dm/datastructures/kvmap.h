/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_KVMAP_H_HEADER_GUARD
#define DM_KVMAP_H_HEADER_GUARD

#include <stdint.h> // uint32_t
#include <new>      // placement-new

#include "common.h" // Heap alloc utils.

#include "../common/common.h" // DM_INLINE
#include "../check.h"         // DM_CHECK

#include "../../../3rdparty/bx/allocator.h" // dm::ReallocatorI

#include "set.h"

namespace dm
{
    template <typename Ty/*arithmetic type*/, uint16_t MaxKeyT>
    struct KeyValueMapT
    {
        KeyValueMapT()
        {
        }

        #include "kvmap_inline_impl.h"

        uint16_t count()
        {
            return m_set.count();
        }

        uint16_t max()
        {
            return MaxKeyT;
        }

    private:
        SetT<MaxKeyT> m_set;
        Ty m_values[MaxKeyT];
    };

    template <typename Ty/*arithmetic type*/>
    struct KeyValueMap
    {
        // Uninitialized state, init() needs to be called !
        KeyValueMap()
        {
            m_memoryBlock = NULL;
        }

        KeyValueMap(uint16_t _max, dm::ReallocatorI* _reallocator)
        {
            init(_max, _reallocator);
        }

        KeyValueMap(uint16_t _max, void* _mem, dm::AllocatorI* _allocator)
        {
            init(_max, _mem, _allocator);
        }

        ~KeyValueMap()
        {
            destroy();
        }

        enum
        {
            SizePerElement = sizeof(Ty) + Set::SizePerElement,
        };

        static inline uint32_t sizeFor(uint16_t _max)
        {
            return _max*SizePerElement;
        }

        // Allocates memory internally.
        void init(uint16_t _max, dm::ReallocatorI* _reallocator)
        {
            m_max = _max;
            m_memoryBlock = DM_ALLOC(_reallocator, sizeFor(_max));
            m_reallocator = _reallocator;
            m_cleanup = true;

            void* ptr = m_memoryBlock;
            ptr = m_set.init(_max, ptr, (dm::AllocatorI*)_reallocator);
            m_values = (Ty*)ptr;
        }

        // Uses externally allocated memory.
        void* init(uint16_t _max, void* _mem, dm::AllocatorI* _allocator = NULL)
        {
            m_max = _max;
            m_memoryBlock = _mem;
            m_allocator = _allocator;
            m_cleanup = false;

            void* ptr = m_memoryBlock;
            ptr = m_set.init(_max, ptr);
            m_values = (Ty*)ptr;

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
                m_set.destroy();
                if (m_cleanup)
                {
                    DM_FREE(m_reallocator, m_memoryBlock);
                }
                m_memoryBlock = NULL;
            }
        }

        #include "kvmap_inline_impl.h"

        uint16_t count()
        {
            return m_set.count();
        }

        uint16_t max()
        {
            return m_max;
        }

        dm::AllocatorI* allocator()
        {
            return m_allocator;
        }

    private:
        Set m_set;
        Ty* m_values;
        uint16_t m_max;
        union
        {
            dm::AllocatorI*   m_allocator;
            dm::ReallocatorI* m_reallocator;
        };
        bool m_cleanup;
        void* m_memoryBlock;
    };

} // namespace dm

#endif // DM_KVMAP_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
