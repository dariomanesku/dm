/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <stdint.h>
    #include "../check.h"
    #include "../allocatori.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_DENSESET_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_DENSESET_H_HEADER_GUARD
#   define DM_DENSESET_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    template <typename DenseSetStorageTy>
    struct DenseSetImpl : DenseSetStorageTy
    {
        /// Expected interface:
        ///
        ///     struct DenseSetStorageTemplate
        ///     {
        ///         typedef typename dm::bestfit_type<MaxElementsT>::type ElementType;
        ///         ElementType* dense();
        ///         ElementType* sparse();
        ///         ElementType  max();
        ///     };
        typedef typename DenseSetStorageTy::ElementType ElemTy;
        using DenseSetStorageTy::dense;
        using DenseSetStorageTy::sparse;
        using DenseSetStorageTy::max;

        DenseSetImpl() : DenseSetStorageTy()
        {
            m_num = 0;
        }

        ElemTy insert(ElemTy _val)
        {
            DM_CHECK(m_num < max(), "DenseSetImpl::insert() - 0 | %d, %d", m_num, max());
            DM_CHECK(_val < max(),  "DenseSetImpl::insert() - 1 | %d, %d", _val,  max());

            if (contains(_val))
            {
                return indexOf(_val);
            }

            const ElemTy index = m_num++;
            dense()[index] = _val;
            sparse()[_val] = index;

            return index;
        }

        ElemTy safeInsert(ElemTy _val)
        {
            if (_val < max())
            {
                return insert(_val);
            }

            return TyInfo<ElemTy>::Max();
        }

        bool contains(ElemTy _val)
        {
            DM_CHECK(_val < max(), "DenseSetImpl::contains() - 0 | %d, %d", _val, max());

            const ElemTy index = sparse()[_val];

            return (index < m_num && dense()[index] == _val);
        }

        ElemTy indexOf(ElemTy _val)
        {
            DM_CHECK(_val < max(), "DenseSetImpl::indexOf() | %d, %d", _val, max());

            return sparse()[_val];
        }

        ElemTy getValueAt(size_t _idx)
        {
            DM_CHECK(_idx < max(), "DenseSetImpl::getValueAt() | %zu, %d", _idx, max());

            return dense()[_idx];
        }

        bool remove(ElemTy _val)
        {
            DM_CHECK(_val < max(), "DenseSetImpl::remove() - 0 | %d, %d", _val, max());
            DM_CHECK(m_num < max(), "DenseSetImpl::remove() - 1 | %d, %d", m_num, max());

            if (!contains(_val))
            {
                return false;
            }

            const ElemTy index = sparse()[_val];
            const ElemTy last = dense()[--m_num];

            DM_CHECK(index < max(), "DenseSetImpl::remove() - 2 | %d, %d", index, max());
            DM_CHECK(last < max(), "DenseSetImpl::remove() - 3 | %d, %d", last, max());

            dense()[index] = last;
            sparse()[last] = index;

            return true;
        }

        uint32_t count()
        {
            return m_num;
        }

        void reset()
        {
            m_num = 0;
        }

    private:
        uint32_t m_num;
    };

    template <typename ElemTy, uint32_t MaxT>
    struct DenseSetStorageT
    {
        typedef ElemTy ElementType;

        ElemTy* dense()
        {
            return m_dense;
        }

        ElemTy* sparse()
        {
            return m_sparse;
        }

        ElemTy max()
        {
            return MaxT;
        }

    private:
        ElemTy m_dense[MaxT];
        ElemTy m_sparse[MaxT];
    };

    template <typename ElemTy>
    struct DenseSetStorageExt
    {
        typedef ElemTy ElementType;

        enum
        {
            SizePerElement = 2*sizeof(ElemTy)
        };

        static uint32_t sizeFor(uint32_t _max)
        {
            return _max*SizePerElement;
        }

        DenseSetStorageExt()
        {
            m_max = 0;
            m_dense = NULL;
            m_sparse = NULL;
        }

        uint8_t* init(uint32_t _max, uint8_t* _mem)
        {
            const uint32_t haSize = _max*sizeof(ElemTy);

            m_max = _max;
            m_dense  = (ElemTy*)_mem;
            m_sparse = (ElemTy*)((uint8_t*)_mem + haSize);

            return ((uint8_t*)_mem + 2*haSize);
        }

        ElemTy* dense()
        {
            return m_dense;
        }

        ElemTy* sparse()
        {
            return m_sparse;
        }

        uint32_t max()
        {
            return m_max;
        }

    private:
        uint32_t m_max;
        ElemTy* m_dense;
        ElemTy* m_sparse;
    };

    extern CrtCallocator g_crtCallocator;

    template <typename ElemTy>
    struct DenseSetStorage
    {
        typedef ElemTy ElementType;

        enum
        {
            SizePerElement = 2*sizeof(ElemTy)
        };

        static uint32_t sizeFor(uint32_t _max)
        {
            return _max*SizePerElement;
        }

        DenseSetStorage()
        {
            m_max = 0;
            m_dense = NULL;
            m_sparse = NULL;
        }

        ~DenseSetStorage()
        {
            destroy();
        }

        void init(uint32_t _max, AllocatorI* _allocator = &g_crtCallocator)
        {
            const uint32_t haSize = _max*sizeof(ElemTy);
            void* mem = DM_ALLOC(_allocator, 2*haSize);

            m_max = _max;
            m_dense = (ElemTy*)mem;
            m_sparse = (ElemTy*)((uint8_t*)mem + haSize);

            m_allocator = _allocator;
        }

        void destroy()
        {
            if (NULL != m_dense)
            {
                DM_FREE(m_allocator, m_dense);
                m_dense = NULL;
                m_sparse = NULL;
            }
        }

        ElemTy* dense()
        {
            return m_dense;
        }

        ElemTy* sparse()
        {
            return m_sparse;
        }

        uint32_t max()
        {
            return m_max;
        }

    private:
        uint32_t m_max;
        ElemTy* m_dense;
        ElemTy* m_sparse;
        AllocatorI* m_allocator;
    };

    template <uint32_t MaxT>                  struct DenseSetT   : DenseSetImpl< DenseSetStorageT<typename dm::bestfit_type<MaxT>::type, MaxT> > { };
    template <typename ElemTy, uint32_t MaxT> struct DenseSetTy  : DenseSetImpl< DenseSetStorageT<ElemTy, MaxT> > { };
    template <typename ElemTy>                struct DenseSetExt : DenseSetImpl< DenseSetStorageExt<ElemTy> > { };
    template <typename ElemTy>                struct DenseSet    : DenseSetImpl< DenseSetStorage<ElemTy> > { };
    template <typename ElemTy>                struct DenseSetH   : DenseSetExt<ElemTy> { AllocatorI* m_allocator; };

} // namespace DM_NAMESPACE
#   endif // DM_DENSESET_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
