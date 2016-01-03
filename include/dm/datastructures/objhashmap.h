/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_OBJHASHMAP_H_HEADER_GUARD
#define DM_OBJHASHMAP_H_HEADER_GUARD

#include <stdint.h> // uint32_t
#include <new>      // placement-new

#include "../common/common.h" // DM_INLINE
#include "../check.h"         // DM_CHECK
#include "../misc.h"          // dm::TyInfo<>
#include "../hash.h"          // dm::hash
#include "../misc.h"          // dm::isPowTwo
#include "../compiletime.h"   // dm_staticAssert(), dm::is_powtwo<>::value

#include "common.h" // Heap alloc utils.
#include "handlealloc.h"
#include "hashmap.h"

namespace dm
{
    template <uint8_t KeyLen, typename ValTy, uint32_t MaxT_PowTwo, uint8_t EntriesPerSlot_PowTwo=2
            , DM_ENABLE_IF(MaxT_PowTwo,           is_powtwo)
            , DM_ENABLE_IF(EntriesPerSlot_PowTwo, is_powtwo)
            >
    struct ObjHashMapT
    {
        typedef typename dm::bestfit_type<MaxT_PowTwo*EntriesPerSlot_PowTwo>::type HandleType;

        ObjHashMapT()
        {
            dm_staticAssert(is_powtwo<MaxT_PowTwo>::value);
        }

        #include "objhashmap_inline_impl.h"

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

        HashMapT<KeyLen, HandleType, MaxT_PowTwo*EntriesPerSlot_PowTwo> m_hashMap;
        HandleAllocT<MaxT_PowTwo>                                       m_handleAlloc;
        ValTy                                                           m_objects[MaxT_PowTwo];
    };

    template <uint8_t KeyLen, typename ValTy>
    struct ObjHashMap
    {
        typedef uint32_t HandleType;

        // Uninitialized state, init() needs to be called !
        ObjHashMap()
        {
            m_memoryBlock = NULL;
        }

        ObjHashMap(uint32_t _maxPowTwo, uint8_t _entriesPerSlotPowTwo, dm::ReallocatorI* _reallocator)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));
            DM_ASSERT(dm::isPowTwo(_entriesPerSlotPowTwo));

            init(_maxPowTwo, _entriesPerSlotPowTwo, _reallocator);
        }

        ObjHashMap(uint32_t _maxPowTwo, uint8_t _entriesPerSlotPowTwo, void* _mem, dm::AllocatorI* _allocator)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));
            DM_ASSERT(dm::isPowTwo(_entriesPerSlotPowTwo));

            init(_maxPowTwo, _entriesPerSlotPowTwo, _mem, _allocator);
        }

        ~ObjHashMap()
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
            SizePerElement = sizeof(ValTy) + HandleAlloc<HandleType>::SizePerElement + HashMap<KeyLen, HandleType>::SizePerElement,
            SizePerAdditionalEntry = HashMap<KeyLen, HandleType>::SizePerElement,
        };

        static inline uint32_t sizeFor(uint32_t _maxPowTwo, uint8_t _entriesPerSlotPowTwo)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));
            DM_ASSERT(dm::isPowTwo(_entriesPerSlotPowTwo));

            return _maxPowTwo*(SizePerElement + SizePerAdditionalEntry*(DM_MAX(0, _entriesPerSlotPowTwo-1)));
        }

        // Allocates memory internally.
        void init(uint32_t _maxPowTwo, uint8_t _entriesPerSlotPowTwo, dm::ReallocatorI* _reallocator)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));
            DM_ASSERT(dm::isPowTwo(_entriesPerSlotPowTwo));

            m_max = _maxPowTwo;
            m_entriesPerSlot = _entriesPerSlotPowTwo;
            m_memoryBlock = (UsedKeyVal*)DM_ALLOC(_reallocator, sizeFor(_maxPowTwo, _entriesPerSlotPowTwo));
            m_reallocator = _reallocator;
            m_cleanup = true;

            void* ptr = m_memoryBlock;
            ptr = m_hashMap.init(_maxPowTwo*_entriesPerSlotPowTwo, ptr, (dm::AllocatorI*)_reallocator);
            ptr = m_handleAlloc.init(_maxPowTwo, ptr, (dm::AllocatorI*)_reallocator);
            m_objects = (ValTy*)ptr;
        }

        // Uses externally allocated memory.
        void* init(uint32_t _maxPowTwo, uint8_t _entriesPerSlotPowTwo, void* _mem, dm::AllocatorI* _allocator = NULL)
        {
            DM_ASSERT(dm::isPowTwo(_maxPowTwo));
            DM_ASSERT(dm::isPowTwo(_entriesPerSlotPowTwo));

            m_max = _maxPowTwo;
            m_entriesPerSlot = _entriesPerSlotPowTwo;
            m_memoryBlock = _mem;
            m_allocator = _allocator;
            m_cleanup = false;

            void* ptr = m_memoryBlock;
            ptr = m_hashMap.init(_maxPowTwo*_entriesPerSlotPowTwo, ptr);
            ptr = m_handleAlloc.init(_maxPowTwo, ptr);
            m_objects = (ValTy*)ptr;

            void* end = (void*)((uint8_t*)_mem + sizeFor(_maxPowTwo, _entriesPerSlotPowTwo));
            return end;
        }

        bool isInitialized() const
        {
            return (NULL != m_memoryBlock);
        }

        void reinit(uint32_t _maxPowTwo, dm::ReallocatorI* _reallocator)
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
            if (NULL != m_memoryBlock)
            {
                for (uint32_t ii = 0, end = this->count(); ii < end; ++ii)
                {
                    ValTy* obj = this->getValueAt(ii);
                    obj->~ValTy();
                }
                m_handleAlloc.destroy();
                m_hashMap.destroy();
                if (m_cleanup)
                {
                    DM_FREE(m_reallocator, m_memoryBlock);
                }
                m_memoryBlock = NULL;
            }
        }

        #include "objhashmap_inline_impl.h"

        uint32_t max() const
        {
            return m_max;
        }

        dm::AllocatorI* allocator()
        {
            return m_allocator;
        }

    private:
        enum //TODO: use these from HashMap.
        {
            Unused     = 0xff,
            Used       = 0x00,
            InvalidIdx = UINT32_MAX,
        };

        uint32_t m_max;
        uint8_t m_entriesPerSlot;
        HashMap<KeyLen, HandleType> m_hashMap;
        HandleAlloc<HandleType> m_handleAlloc;
        ValTy* m_objects;
        union
        {
            dm::AllocatorI*   m_allocator;
            dm::ReallocatorI* m_reallocator;
        };
        bool m_cleanup;
        void* m_memoryBlock;
    };

    /// Notice: used only for Ty == ObjHashMap.
    template <typename Ty>
    DM_INLINE Ty* create(uint32_t _maxPowTwo, uint8_t _entriesPerSlotPowTwo, void* _mem, dm::AllocatorI* _memDeallocator)
    {
        return ::new (_mem) Ty(_maxPowTwo, _entriesPerSlotPowTwo, (uint8_t*)_mem + sizeof(Ty), _memDeallocator);
    }

    /// Notice: used only for Ty == ObjHashMap.
    template <typename Ty>
    DM_INLINE Ty* create(uint32_t _maxPowTwo, uint8_t _entriesPerSlotPowTwo, dm::AllocatorI* _allocator)
    {
        uint8_t* ptr = (uint8_t*)DM_ALLOC(_allocator, sizeof(Ty) + Ty::sizeFor(_maxPowTwo, _entriesPerSlotPowTwo));
        return create<Ty>(_maxPowTwo, _entriesPerSlotPowTwo, ptr, _allocator);
    }

} // namespace dm

#endif // DM_HASHMAP_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */


