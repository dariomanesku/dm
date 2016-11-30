/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <stdint.h>
    #include "../check.h"
    #include "../compiletime.h" // bestfit_type<>::type, TyInfo<>::Max()
    #include "../ng/allocatori.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_IDXALLOC_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_IDXALLOC_H_HEADER_GUARD
#   define DM_IDXALLOC_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    template <typename IdxAllocStorageTy>
    struct IdxAllocImpl : IdxAllocStorageTy
    {
        /// Expected interface:
        ///     struct IdxAllocStorageTemplate
        ///     {
        ///         typedef typename dm::bestfit_type<MaxHandlesT>::type IdxType;
        ///         IdxType* indices();
        ///         IdxType  max();
        ///     };
        typedef typename IdxAllocStorageTy::IdxType IdxTy;
        using IdxAllocStorageTy::indices;
        using IdxAllocStorageTy::max;

        IdxAllocImpl() : IdxAllocStorageTy()
        {
        }

        void init()
        {
            doReset();
        }

        void doReset()
        {
            m_count = 0;
            for (uint32_t ii = 0, end = max(); ii < end; ++ii)
            {
                indices()[ii] = ii;
            }
        }

        IdxTy alloc()
        {
            DM_CHECK(m_count < max(), "IdxAllocImpl::alloc() | %d, %d", m_count, max());

            return indices()[m_count++];
        }

        IdxTy* alloc(uint32_t _count)
        {
            DM_CHECK((m_count + _count) < max(), "IdxAllocImpl::alloc(_count) | %d, %d", m_count + _count, max());

            const uint32_t curr = m_count;
            m_count += _count;
            return &indices()[curr];
        }

        void removeAt(uint32_t _at)
        {
            /// Swap '_at' and 'last'.
            IdxTy tmp = indices()[_at];
            indices()[_at] = indices()[--m_count];
            indices()[m_count] = tmp;
        }

        static int cmpAsc(const void* _a, const void* _b)
        {
            return (*(IdxTy*)_a - *(IdxTy*)_b);
        }

        void sort()
        {
            if (m_count <= 1)
            {
                return;
            }

            qsort(indices(), m_count, sizeof(IdxTy), cmpAsc);
        }

        IdxTy getAt(uint32_t _idx)
        {
            DM_CHECK(_idx < max(), "IdxAllocImpl::get() | %d, %d", _idx, max());

            return indices()[_idx];
        }

        IdxTy& operator[](uint32_t _idx)
        {
            DM_CHECK(_idx < max(), "IdxAllocImpl::operator[]() ref | %d, %d", _idx, max());

            return indices()[_idx];
        }

        const IdxTy& operator[](uint32_t _idx) const
        {
            DM_CHECK(_idx < max(), "IdxAllocImpl::operator[]() const | %d, %d", _idx, max());

            return indices()[_idx];
        }

        IdxTy count()
        {
            return m_count;
        }

    private:
        IdxTy m_count;
    };

    template <uint32_t MaxHandlesT>
    struct IdxAllocStorageT
    {
        typedef typename dm::bestfit_type<MaxHandlesT>::type IdxType;

        IdxType* indices()
        {
            return m_indices;
        }

        IdxType max()
        {
            return MaxHandlesT;
        }

    private:
        IdxType m_indices[MaxHandlesT];
    };

    template <typename IdxTy=uint16_t>
    struct IdxAllocStorageExt
    {
        typedef IdxTy IdxType;

        static uint32_t sizeFor(uint32_t _max)
        {
            return 2*_max*sizeof(IdxType);
        }

        IdxAllocStorageExt()
        {
            m_max = 0;
            m_indices = NULL;
        }

        uint8_t* initStorage(uint32_t _max, uint8_t* _mem)
        {
            const uint32_t idxSize = _max*sizeof(IdxType);

            m_max = _max;
            m_indices = (IdxType*)_mem;

            return _mem + idxSize;
        }

        IdxType* indices()
        {
            return m_indices;
        }

        IdxType max()
        {
            return m_max;
        }

    private:
        uint32_t m_max;
        IdxType* m_indices;
    };

    extern CrtAllocator g_crtAllocator;

    template <typename IdxTy=uint16_t>
    struct IdxAllocStorage
    {
        typedef IdxTy IdxType;

        static uint32_t sizeFor(uint32_t _max)
        {
            return 2*_max*sizeof(IdxType);
        }

        IdxAllocStorage()
        {
            m_max = 0;
            m_indices = NULL;
        }

        ~IdxAllocStorage()
        {
            destroy();
        }

        void initStorage(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            const uint32_t idxSize = _max*sizeof(IdxType);
            void* mem = dm_alloc(idxSize, _allocator);

            m_max = _max;
            m_indices = (IdxType*)mem;

            m_allocator = _allocator;
        }

        void destroy()
        {
            if (NULL != m_indices)
            {
                dm_free(m_indices, m_allocator);
                m_indices = NULL;
            }
        }

        IdxType* indices()
        {
            return m_indices;
        }

        IdxType max()
        {
            return m_max;
        }

    private:
        uint32_t m_max;
        IdxType* m_indices;
        AllocatorI* m_allocator;
    };

    template <uint32_t MaxHandlesT>
    struct IdxAllocT : IdxAllocImpl< IdxAllocStorageT<MaxHandlesT> >
    {
        typedef IdxAllocImpl< IdxAllocStorageT<MaxHandlesT> > Base;

        IdxAllocT()
        {
            Base::init();
        }
    };

    template <typename IdxTy=uint16_t>
    struct IdxAllocExt : IdxAllocImpl< IdxAllocStorageExt<IdxTy> >
    {
        typedef IdxAllocImpl< IdxAllocStorageExt<IdxTy> > Base;

        uint8_t* init(uint32_t _max, uint8_t* _mem)
        {
            uint8_t* ptr = Base::initStorage(_max, _mem);
            Base::init();

            return ptr;
        }
    };

    template <typename IdxTy=uint16_t>
    struct IdxAlloc : IdxAllocImpl< IdxAllocStorage<IdxTy> >
    {
        typedef IdxAllocImpl< IdxAllocStorage<IdxTy> > Base;

        void init(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            Base::initStorage(_max, _allocator);
            Base::init();
        }
    };

    template <typename IdxTy=uint16_t>
    struct IdxAllocH : IdxAllocExt<IdxTy>
    {
        AllocatorI* m_allocator;
    };

} // namespace DM_NAMESPACE
#   endif // DM_IDXALLOC_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
