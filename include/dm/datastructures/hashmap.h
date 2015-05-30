/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_HASHMAP_H_HEADER_GUARD
#define DM_HASHMAP_H_HEADER_GUARD

#include <stdint.h> // uint32_t
#include <string.h> // memset
#include <new>      // placement-new

#include "common.h" // Heap alloc utils.

#include "../common/common.h" // DM_INLINE
#include "../check.h"         // DM_CHECK
#include "../compiletime.h"   // dm::TyInfo<>, DM_ENABLE_IF
#include "../hash.h"          // dm::hash
#include "../misc.h"          // dm::isPowTwo

#include "../../../3rdparty/bx/allocator.h" // bx::ReallocatorI

namespace dm
{
    template <uint8_t KeyLen, typename ValTy, uint32_t MaxT_PowTwo, DM_ENABLE_IF(MaxT_PowTwo, is_powtwo)>
    struct HashMapT
    {
        HashMapT()
        {
            DM_ASSERT(dm::isPowTwo(MaxT_PowTwo));

            memset(m_ukv, 0xff, sizeof(m_ukv));
        }

        #include "hashmap_inline_impl.h"

        uint32_t max() const
        {
            return MaxT_PowTwo;
        }

    private:
        enum
        {
            Unused     = 0xff,
            Used       = 0x00,
            InvalidIdx = UINT32_MAX,
        };

        struct UsedKeyVal
        {
            uint8_t m_used;
            uint8_t m_key[KeyLen];
            ValTy   m_val;
        };

        UsedKeyVal m_ukv[MaxT_PowTwo];
    };

    template <uint8_t KeyLen, typename ValTy>
    struct HashMap
    {
        // Uninitialized state, init() needs to be called !
        HashMap()
        {
            m_ukv = NULL;
        }

        HashMap(uint32_t _maxPowTwo, bx::ReallocatorI* _reallocator)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));

            init(_maxPowTwo, _reallocator);
        }

        HashMap(uint32_t _maxPowTwo, void* _mem, bx::AllocatorI* _allocator)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));

            init(_maxPowTwo, _mem, _allocator);
        }

        ~HashMap()
        {
            destroy();
        }

        struct UsedKeyVal
        {
            uint8_t m_used;
            uint8_t m_key[KeyLen];
            ValTy   m_val;
        };

        enum
        {
            SizePerElement = sizeof(UsedKeyVal),
        };

        static inline uint32_t sizeFor(uint32_t _maxPowTwo)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));

            return _maxPowTwo*SizePerElement;
        }

        // Allocates memory internally.
        void init(uint32_t _maxPowTwo, bx::ReallocatorI* _reallocator)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));

            m_max = _maxPowTwo;
            m_ukv = (UsedKeyVal*)BX_ALLOC(_reallocator, sizeFor(_maxPowTwo));
            m_reallocator = _reallocator;
            m_cleanup = true;

            memset(m_ukv, 0xff, _maxPowTwo*sizeof(UsedKeyVal));
        }

        // Uses externally allocated memory.
        void* init(uint32_t _maxPowTwo, void* _mem, bx::AllocatorI* _allocator = NULL)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));

            m_max = _maxPowTwo;
            m_ukv = (UsedKeyVal*)_mem;
            m_allocator = _allocator;
            m_cleanup = false;

            memset(m_ukv, 0xff, _maxPowTwo*sizeof(UsedKeyVal));

            void* end = (void*)((uint8_t*)_mem + sizeFor(_maxPowTwo));
            return end;
        }

        bool isInitialized() const
        {
            return (NULL != m_ukv);
        }

        void reinit(uint32_t _maxPowTwo, bx::ReallocatorI* _reallocator)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));

            if (isInitialized())
            {
                destroy();
            }

            init(_maxPowTwo, _reallocator);
        }

        void destroy()
        {
            if (m_cleanup && NULL != m_ukv)
            {
                BX_FREE(m_reallocator, m_ukv);
                m_ukv = NULL;
            }
        }

        #include "hashmap_inline_impl.h"

        uint32_t max() const
        {
            return m_max;
        }

        bx::AllocatorI* allocator()
        {
            return m_allocator;
        }

    private:
        enum
        {
            Unused     = 0xff,
            Used       = 0x00,
            InvalidIdx = UINT32_MAX,
        };

        uint32_t m_max;
        UsedKeyVal* m_ukv;
        union
        {
            bx::AllocatorI*   m_allocator;
            bx::ReallocatorI* m_reallocator;
        };
        bool m_cleanup;
    };

} // namespace dm

#endif // DM_HASHMAP_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
