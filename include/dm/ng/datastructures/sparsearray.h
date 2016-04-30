/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_SPARSEARRAY_HEADER_GUARD
#define DM_NG_SPARSEARRAY_HEADER_GUARD

#include <new>
#include <dm/check.h>
#include <dm/ng/allocator.h>
#include "handlealloc.h"

namespace dm { namespace ng {

template <typename SparseArrayStorageTy>
struct SparseArrayImpl : SparseArrayStorageTy
{
    /// Expected interface:
    ///
    ///     template <typename ObjTy>
    ///     struct SparseArrayStorageT
    ///     {
    ///         typedef ObjTy ObjectType;
    ///         typedef IdxAllocT<MaxT> IdxAllocType;
    ///
    ///         ObjTy* objects();
    ///         IdxAllocType* indices();
    ///         uint32_t max();
    ///     };
    typedef typename SparseArrayStorageTy::ObjectType   ObjTy;
    typedef typename SparseArrayStorageTy::IdxAllocType IdxAllocTy;
    using SparseArrayStorageTy::objects;
    using SparseArrayStorageTy::indices;
    using SparseArrayStorageTy::max;

    SparseArrayImpl() : SparseArrayStorageTy()
    {
    }

    ObjTy* addNew()
    {
        const uint32_t idx = (uint32_t)indices()->alloc();
        DM_CHECK(idx < max(), "SparseArrayImpl::addNew() | %d, %d", idx, max());

        ObjTy* dst = &objects()[idx];
        dst = ::new (dst) ObjTy();
        return dst;
    }

    uint32_t addCopy(const ObjTy* _obj)
    {
        const uint32_t idx = (uint32_t)indices()->alloc();
        DM_CHECK(idx < max(), "SparseArrayImpl::addCopy() | %d, %d", idx, max());

        ObjTy* dst = &objects()[idx];
        dst = ::new (dst) ObjTy(*_obj);
        return idx;
    }

    bool contains(const ObjTy* _obj)
    {
        return (&objects()[0] <= _obj && _obj < &objects()[max()]);
    }

    uint32_t getIdxOfObj(const ObjTy* _obj)
    {
        DM_CHECK(contains(_obj), "SparseArrayImpl::getIdxOf() | Object not from the list.");

        return uint32_t(_obj - objects());
    }

    uint32_t getIdxOf(uint32_t _handle)
    {
        DM_CHECK(_handle < max(), "SparseArrayImpl::getIdxOf() #1 | %d, %d", _handle, max());
        const uint32_t idx = (uint32_t)indices()->getAt(_handle);
        DM_CHECK(idx < max(), "SparseArrayImpl::getIdxOf() #2 | %d, %d", idx, max());

        return idx;
    }

    ObjTy* get(uint32_t _handle)
    {
        const uint32_t idx = getIdxOf(_handle);

        return &objects()[idx];
    }

    private:
    ObjTy* getObjAt_impl(uint32_t _idx)
    {
        DM_CHECK(_idx < max(), "SparseArrayImpl::getObjAt_impl() | %d, %d", _idx, max());

        return &objects()[_idx];
    } public:

    ObjTy* getAt(uint32_t _idx)
    {
        return getObjAt_impl(_idx);
    }

    ObjTy& operator[](uint32_t _idx)
    {
        return *getObjAt_impl(_idx);
    }

    const ObjTy& operator[](uint32_t _idx) const
    {
        return *getObjAt_impl(_idx);
    }

    void removeAt(uint32_t _idx)
    {
        DM_CHECK(_idx < max(), "SparseArrayImpl::removeAt() | %d, %d", _idx, max());

        objects()[_idx].~ObjTy();
        indices()->removeAt(_idx);
    }

    void removeObj(ObjTy* _obj)
    {
        const uint32_t idx = getIdxOfObj(_obj);
        removeAt(idx);
    }

    void remove(uint32_t _handle)
    {
        const uint32_t idx = getIdxOf(_handle);
        removeAt(idx);
    }

    void removeAll()
    {
        for (uint32_t hh = count(); hh--; )
        {
            remove(hh);
        }
    }

    void sort()
    {
        indices()->sort();
    }

    void compact()
    {
        const uint32_t end = count();
        if (end <= 1)
        {
            return;
        }

        sort();

        uint32_t idx = 0;
        uint32_t prev;
        uint32_t curr = indices()->getAt(0);

        bool inOrder = (0 == curr);
        for (;;)
        {
            prev = curr;

            if (inOrder)
            {
                idx++;
            }
            else
            {
                uint32_t dst = 0;
                uint32_t src = curr;

                uint32_t cnt = 1;
                for (; cnt < end; ++cnt)
                {
                    const uint32_t next = indices()->getAt(cnt);
                    if (next-prev != 1)
                    {
                        break;
                    }
                    prev = next;
                }

                if (cnt > curr)
                {
                    memmove(&objects()[dst], &objects()[src], cnt*sizeof(ObjTy));
                }
                else
                {
                    memcpy(&objects()[dst], &objects()[src], cnt*sizeof(ObjTy));
                }

                idx += cnt;
            }

            if (idx >= end)
            {
                break;
            }

            curr = indices()->getAt(idx);
            const uint32_t diff = curr - prev;
            inOrder = (1 == diff);
        }
    }

    void zero()
    {
        memset(objects(), 0, max()*sizeof(ObjTy));
    }

    void doReset()
    {
        indices()->doReset();
    }

    uint32_t count()
    {
        return indices()->count();
    }
};

template <typename ObjTy, uint32_t MaxT>
struct SparseArrayStorageT
{
    typedef ObjTy ObjectType;
    typedef IdxAllocT<MaxT> IdxAllocType;

    ObjTy* objects()
    {
        return m_objects;
    }

    IdxAllocType* indices()
    {
        return &m_indices;
    }

    uint32_t max()
    {
        return MaxT;
    }

    IdxAllocType m_indices;
    ObjTy m_objects[MaxT];
};

template <typename ObjTy>
struct SparseArrayStorageExt
{
    typedef ObjTy ObjectType;
    typedef IdxAllocExt<uint32_t> IdxAllocType;

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(ObjTy) + IdxAllocType::sizeFor(_max);
    }

    SparseArrayStorageExt()
    {
        m_max = 0;
        m_objects = NULL;
    }

    uint8_t* init(uint32_t _max, uint8_t* _mem)
    {
        uint8_t* objBegin   = (uint8_t*)_mem;
        uint8_t* handleBegin = (uint8_t*)_mem + _max*sizeof(ObjTy);

        m_max = _max;
        m_objects = (ObjTy*)objBegin;
        uint8_t* end = m_indices.init(_max, handleBegin);

        return end;
    }

    ObjTy* objects()
    {
        return m_objects;
    }

    IdxAllocType* indices()
    {
        return &m_indices;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint32_t m_max;
    ObjTy* m_objects;
    IdxAllocType m_indices;
};

template <typename ObjTy>
struct SparseArrayStorage
{
    typedef ObjTy ObjectType;
    typedef IdxAllocExt<uint32_t> IdxAllocType;

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(ObjTy) + IdxAllocType::sizeFor(_max);
    }

    SparseArrayStorage()
    {
        m_max = 0;
        m_objects = NULL;
    }

    void init(uint32_t _max, ReallocFn _reallocFn = &::realloc)
    {
        const uint32_t totalSize = sizeFor(_max);
        void* mem = dm_alloc(totalSize, _reallocFn);

        uint8_t* objBegin    = (uint8_t*)mem;
        uint8_t* handleBegin = (uint8_t*)mem + _max*sizeof(ObjTy);

        m_max = _max;
        m_objects = (ObjTy*)objBegin;
        m_indices.init(_max, handleBegin);

        m_reallocFn = _reallocFn;
    }

    void destroy()
    {
        if (NULL != m_objects)
        {
            dm_free(m_objects, m_reallocFn);
            m_objects = NULL;
        }
    }

    ObjTy* objects()
    {
        return m_objects;
    }

    IdxAllocType* indices()
    {
        return &m_indices;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint32_t m_max;
    ObjTy* m_objects;
    IdxAllocType m_indices;
    ReallocFn m_reallocFn;
};

template <typename ObjTy, uint32_t MaxT> struct SparseArrayT   : SparseArrayImpl< SparseArrayStorageT<ObjTy, MaxT> > { };
template <typename ObjTy>                struct SparseArrayExt : SparseArrayImpl< SparseArrayStorageExt<ObjTy>     > { };
template <typename ObjTy>                struct SparseArray    : SparseArrayImpl< SparseArrayStorage<ObjTy>        > { };
template <typename ObjTy>                struct SparseArrayH   : SparseArrayExt<ObjTy> { ReallocFn m_reallocFn; };

} //namespace ng
} //namespace dm

#endif // DM_NG_SPARSEARRAY_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
