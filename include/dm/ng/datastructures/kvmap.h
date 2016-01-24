/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_KVMAP_HEADER_GUARD
#define DM_NG_KVMAP_HEADER_GUARD

#include <stdint.h>
#include <dm/check.h>
#include <dm/ng/allocator.h>
#include "set.h"

namespace dm { namespace ng {

template <typename KvMapStorage>
struct KvMapImpl : KvMapStorage
{
    /// Expected interface:
    ///
    ///     struct KvMapStorageT
    ///     {
    ///         typedef ValTy ValueType;
    ///         typedef SetExt Set;
    ///
    ///         ValTy* values();
    ///         Set* keySet();
    ///         uint16_t max();
    ///     };
    typedef typename KvMapStorage::ValueType ValTy;
    typedef typename KvMapStorage::Set       SetTy;
    using KvMapStorage::values;
    using KvMapStorage::keySet;
    using KvMapStorage::max;

    KvMapImpl() : KvMapStorage()
    {
    }

    void insert(uint16_t _key, ValTy _value)
    {
        DM_CHECK(_key < max(), "KvMapImpl::insert() - 0 | %d, %d", _key, max());

        if (_key < max())
        {
            const uint16_t index = keySet()->insert(_key);

            DM_CHECK(index < max(), "KvMapImpl::insert() - 1 | %d, %d", _key, max());
            values()[index] = _value;
        }
    }

    bool contains(uint16_t _key)
    {
        DM_CHECK(_key < max(), "KvMapImpl::contains() | %d, %d", _key, max());

        return keySet()->contains(_key);
    }

    ValTy valueOf(uint16_t _key)
    {
        DM_CHECK(keySet()->indexOf(_key) < max(), "KvMapImpl::valueOf() | %d, %d, %d", _key, keySet()->indexOf(_key), max());

        return values()[keySet()->indexOf(_key)];
    }

    uint16_t getKeyAt(uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "KvMapImpl::getKeyAt() | %d, %d", _idx, max());

        return keySet()->getValueAt(_idx);
    }

    ValTy getValueAt(uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "KvMapImpl::getValueAt() | %d, %d", _idx, max());

        return valueOf(getKeyAt(_idx));
    }

    void remove(uint16_t _key)
    {
        DM_CHECK(_key < max(), "KvMapImpl::remove() | %d, %d", _key, max());

        keySet()->remove(_key);
    }

    void reset()
    {
        keySet()->reset();
    }

    uint16_t count()
    {
        return keySet()->count();
    }

private:
    uint16_t m_last;
};

template <typename ValTy/*arithmetic type*/, uint16_t MaxT>
struct KvMapStorageT
{
    typedef ValTy ValueType;
    typedef SetT<MaxT> Set;

    ValTy* values()
    {
        return m_values;
    }

    Set* keySet()
    {
        return &m_keySet;
    }

    uint16_t max()
    {
        return MaxT;
    }

    Set m_keySet;
    ValTy m_values[MaxT];
};

template <typename ValTy/*arithmetic type*/>
struct KvMapStorageExt
{
    typedef ValTy ValueType;
    typedef SetExt Set;

    static uint16_t sizeFor(uint16_t _max)
    {
        return _max*sizeof(ValTy) + Set::sizeFor(_max);
    }

    KvMapStorageExt()
    {
        m_max = 0;
        m_values = NULL;
    }

    uint8_t* init(uint16_t _max, uint8_t* _mem)
    {
        uint8_t* valBegin   = (uint8_t*)_mem;
        uint8_t* setBegin = (uint8_t*)_mem + _max*sizeof(ValTy);

        m_max = _max;
        m_values = (ValTy*)valBegin;
        uint8_t* end = m_keySet.init(_max, setBegin);

        return end;
    }

    ValTy* values()
    {
        return m_values;
    }

    Set* keySet()
    {
        return &m_keySet;
    }

    uint16_t max()
    {
        return m_max;
    }

    uint16_t m_max;
    ValTy* m_values;
    Set m_keySet;
};

template <typename ValTy/*arithmetic type*/>
struct KvMapStorage
{
    typedef ValTy ValueType;
    typedef SetExt Set;

    static uint16_t sizeFor(uint16_t _max)
    {
        return _max*sizeof(ValTy) + Set::sizeFor(_max);
    }

    KvMapStorage()
    {
        m_max = 0;
        m_values = NULL;
    }

    void init(uint16_t _max, Allocator* _allocator)
    {
        const uint16_t totalSize = sizeFor(_max);
        void* mem = _allocator->m_allocFunc(totalSize);

        uint8_t* valBegin = (uint8_t*)mem;
        uint8_t* setBegin = (uint8_t*)mem + _max*sizeof(ValTy);

        m_max = _max;
        m_values = (ValTy*)valBegin;
        m_keySet.init(_max, setBegin);

        m_freeFunc = _allocator->m_freeFunc;
    }

    void destroy()
    {
        if (NULL != m_values)
        {
            m_freeFunc(m_values);
            m_values = NULL;
        }
    }

    ValTy* values()
    {
        return m_values;
    }

    Set* keySet()
    {
        return &m_keySet;
    }

    uint16_t max()
    {
        return m_max;
    }

    uint16_t m_max;
    ValTy* m_values;
    Set m_keySet;
    FreeFunc m_freeFunc;
};

template <typename ValTy, uint16_t MaxT> struct KvMapT   : KvMapImpl< KvMapStorageT<ValTy, MaxT> > { };
template <typename ValTy>                struct KvMapExt : KvMapImpl< KvMapStorageExt<ValTy>     > { };
template <typename ValTy>                struct KvMap    : KvMapImpl< KvMapStorage<ValTy>        > { };
template <typename ValTy>                struct KvMapH   : KvMapExt<ValTy> { FreeFunc m_freeFunc; };

} //namespace ng
} //namespace dm

#endif // DM_NG_KVMAP_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */

