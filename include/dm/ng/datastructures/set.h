/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_SET_HEADER_GUARD
#define DM_NG_SET_HEADER_GUARD

#include <stdint.h>
#include <dm/check.h>
#include <dm/ng/allocator.h>

namespace dm { namespace ng {

template <typename SetStorage>
struct SetImpl : SetStorage
{
    /// Expected interface:
    ///
    ///     struct SetStorageTemplate
    ///     {
    ///         uint16_t* values();
    ///         uint16_t* indices();
    ///         uint16_t  max();
    ///     };
    using SetStorage::values;
    using SetStorage::indices;
    using SetStorage::max;

    SetImpl() : SetStorage()
    {
        m_num = 0;
    }

    uint16_t insert(uint16_t _val)
    {
        DM_CHECK(m_num < max(), "SetImpl::insert() - 0 | %d, %d", m_num, max());
        DM_CHECK(_val < max(),  "SetImpl::insert() - 1 | %d, %d", _val,  max());

        if (contains(_val))
        {
            return indexOf(_val);
        }

        const uint16_t index = m_num++;
        values()[index] = _val;
        indices()[_val] = index;

        return index;
    }

    uint16_t safeInsert(uint16_t _val)
    {
        if (_val < max())
        {
            return insert(_val);
        }

        return UINT16_MAX;
    }

    bool contains(uint16_t _val)
    {
        DM_CHECK(_val < max(), "SetImpl::contains() - 0 | %d, %d", _val, max());

        const uint16_t index = indices()[_val];

        return (index < m_num && values()[index] == _val);
    }

    uint16_t indexOf(uint16_t _val)
    {
        DM_CHECK(_val < max(), "SetImpl::indexOf() | %d, %d", _val, max());

        return indices()[_val];
    }

    uint16_t getValueAt(uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "SetImpl::getValueAt() | %d, %d", _idx, max());

        return values()[_idx];
    }

    void remove(uint16_t _val)
    {
        DM_CHECK(_val < max(), "SetImpl::remove() - 0 | %d, %d", _val, max());
        DM_CHECK(m_num < max(), "SetImpl::remove() - 1 | %d, %d", m_num, max());

        if (!contains(_val))
        {
            return;
        }

        const uint16_t index = indices()[_val];
        const uint16_t last = values()[--m_num];

        DM_CHECK(index < max(), "SetImpl::remove() - 2 | %d, %d", index, max());
        DM_CHECK(last < max(), "SetImpl::remove() - 3 | %d, %d", last, max());

        values()[index] = last;
        indices()[last] = index;
    }

    uint16_t count()
    {
        return m_num;
    }

    void reset()
    {
        m_num = 0;
    }

private:
    uint16_t m_num;
};

template <uint16_t MaxT>
struct SetStorageT
{
    uint16_t* values()
    {
        return m_values;
    }

    uint16_t* indices()
    {
        return m_indices;
    }

    uint16_t max()
    {
        return MaxT;
    }

private:
    uint16_t m_values[MaxT];
    uint16_t m_indices[MaxT];
};

struct SetStorageExt
{
    static uint32_t sizeFor(uint16_t _max)
    {
        return 2*_max*sizeof(uint16_t);
    }

    SetStorageExt()
    {
        m_max = 0;
        m_values = NULL;
        m_indices = NULL;
    }

    uint8_t* init(uint16_t _max, uint8_t* _mem)
    {
        const uint32_t haSize = _max*sizeof(uint16_t);

        m_max = _max;
        m_values = (uint16_t*)_mem;
        m_indices = (uint16_t*)((uint8_t*)_mem + haSize);

        return _mem + 2*haSize;
    }

    uint16_t* values()
    {
        return m_values;
    }

    uint16_t* indices()
    {
        return m_indices;
    }

    uint16_t max()
    {
        return m_max;
    }

private:
    uint16_t m_max;
    uint16_t* m_values;
    uint16_t* m_indices;
};

struct SetStorage
{
    static uint32_t sizeFor(uint16_t _max)
    {
        return 2*_max*sizeof(uint16_t);
    }

    SetStorage()
    {
        m_max = 0;
        m_values = NULL;
        m_indices = NULL;
    }

    ~SetStorage()
    {
        destroy();
    }

    void init(uint16_t _max, Allocator* _allocator)
    {
        const uint32_t haSize = _max*sizeof(uint16_t);
        void* mem = _allocator->m_allocFunc(2*haSize);

        m_max = _max;
        m_values = (uint16_t*)mem;
        m_indices = (uint16_t*)((uint8_t*)mem + haSize);

        m_freeFunc = _allocator->m_freeFunc;
    }

    void destroy()
    {
        if (NULL != m_values)
        {
            m_freeFunc(m_values);
            m_values = NULL;
            m_indices = NULL;
        }
    }

    uint16_t* values()
    {
        return m_values;
    }

    uint16_t* indices()
    {
        return m_indices;
    }

    uint16_t max()
    {
        return m_max;
    }

private:
    uint16_t m_max;
    uint16_t* m_values;
    uint16_t* m_indices;
    FreeFunc m_freeFunc;
};

template <uint16_t MaxT> struct SetT   : SetImpl< SetStorageT<MaxT> > { };
                         struct SetExt : SetImpl< SetStorageExt     > { };
                         struct Set    : SetImpl< SetStorage        > { };
                         struct SetH   : SetExt { FreeFunc m_freeFunc; };

} //namespace ng
} //namespace dm

#endif // DM_NG_SET_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
