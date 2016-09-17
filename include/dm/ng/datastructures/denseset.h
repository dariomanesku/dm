/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_DENSESET_HEADER_GUARD
#define DM_NG_DENSESET_HEADER_GUARD

#include <stdint.h>
#include <dm/check.h>
#include <dm/ng/allocator.h>

namespace dm { namespace ng {

template <typename DenseSetStorageTy>
struct DenseSetImpl : DenseSetStorageTy
{
    /// Expected interface:
    ///
    ///     struct DenseSetStorageTemplate
    ///     {
    ///         typedef typename dm::bestfit_type<MaxElementsT>::type ElementType;
    ///         ElementType* values();
    ///         ElementType* indices();
    ///         ElementType  max();
    ///     };
    typedef typename DenseSetStorageTy::ElementType ElemTy;
    using DenseSetStorageTy::values;
    using DenseSetStorageTy::indices;
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
        values()[index] = _val;
        indices()[_val] = index;

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

        const ElemTy index = indices()[_val];

        return (index < m_num && values()[index] == _val);
    }

    ElemTy indexOf(ElemTy _val)
    {
        DM_CHECK(_val < max(), "DenseSetImpl::indexOf() | %d, %d", _val, max());

        return indices()[_val];
    }

    ElemTy getValueAt(size_t _idx)
    {
        DM_CHECK(_idx < max(), "DenseSetImpl::getValueAt() | %zu, %d", _idx, max());

        return values()[_idx];
    }

    bool remove(ElemTy _val)
    {
        DM_CHECK(_val < max(), "DenseSetImpl::remove() - 0 | %d, %d", _val, max());
        DM_CHECK(m_num < max(), "DenseSetImpl::remove() - 1 | %d, %d", m_num, max());

        if (!contains(_val))
        {
            return false;
        }

        const ElemTy index = indices()[_val];
        const ElemTy last = values()[--m_num];

        DM_CHECK(index < max(), "DenseSetImpl::remove() - 2 | %d, %d", index, max());
        DM_CHECK(last < max(), "DenseSetImpl::remove() - 3 | %d, %d", last, max());

        values()[index] = last;
        indices()[last] = index;

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

    ElemTy* values()
    {
        return m_values;
    }

    ElemTy* indices()
    {
        return m_indices;
    }

    ElemTy max()
    {
        return MaxT;
    }

private:
    ElemTy m_values[MaxT];
    ElemTy m_indices[MaxT];
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
        m_values = NULL;
        m_indices = NULL;
    }

    uint8_t* init(uint32_t _max, uint8_t* _mem)
    {
        const uint32_t haSize = _max*sizeof(ElemTy);

        m_max = _max;
        m_values  = (ElemTy*)_mem;
        m_indices = (ElemTy*)((uint8_t*)_mem + haSize);

        return ((uint8_t*)_mem + 2*haSize);
    }

    ElemTy* values()
    {
        return m_values;
    }

    ElemTy* indices()
    {
        return m_indices;
    }

    uint32_t max()
    {
        return m_max;
    }

private:
    uint32_t m_max;
    ElemTy* m_values;
    ElemTy* m_indices;
};

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
        m_values = NULL;
        m_indices = NULL;
    }

    ~DenseSetStorage()
    {
        destroy();
    }

    void init(uint32_t _max, ReallocFn _reallocFn = ::realloc)
    {
        const uint32_t haSize = _max*sizeof(ElemTy);
        void* mem = dm_alloc(2*haSize, _reallocFn);

        m_max = _max;
        m_values = (ElemTy*)mem;
        m_indices = (ElemTy*)((uint8_t*)mem + haSize);

        m_reallocFn = _reallocFn;
    }

    void destroy()
    {
        if (NULL != m_values)
        {
            dm_free(m_values, m_reallocFn);
            m_values = NULL;
            m_indices = NULL;
        }
    }

    ElemTy* values()
    {
        return m_values;
    }

    ElemTy* indices()
    {
        return m_indices;
    }

    uint32_t max()
    {
        return m_max;
    }

private:
    uint32_t m_max;
    ElemTy* m_values;
    ElemTy* m_indices;
    ReallocFn m_reallocFn;
};

template <uint32_t MaxT>                  struct DenseSetT   : DenseSetImpl< DenseSetStorageT<typename dm::bestfit_type<MaxT>::type, MaxT> > { };
template <typename ElemTy, uint32_t MaxT> struct DenseSetTy  : DenseSetImpl< DenseSetStorageT<ElemTy, MaxT> > { };
template <typename ElemTy>                struct DenseSetExt : DenseSetImpl< DenseSetStorageExt<ElemTy> > { };
template <typename ElemTy>                struct DenseSet    : DenseSetImpl< DenseSetStorage<ElemTy> > { };
template <typename ElemTy>                struct DenseSetH   : DenseSetExt<ElemTy> { ReallocFn m_reallocFn; };

} //namespace ng
} //namespace dm

#endif // DM_NG_DENSESET_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
