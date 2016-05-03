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
    ///         typedef HandleAllocT<MaxT> HandleAllocType;
    ///
    ///         ObjTy* objects();
    ///         HandleAllocType* handles();
    ///         uint32_t max();
    ///     };
    typedef typename SparseArrayStorageTy::ObjectType ObjTy;
    typedef typename SparseArrayStorageTy::HandleAllocType HandleAllocTy;
    using SparseArrayStorageTy::objects;
    using SparseArrayStorageTy::handles;
    using SparseArrayStorageTy::max;

    SparseArrayImpl() : SparseArrayStorageTy()
    {
    }

    ObjTy* addNew()
    {
        const uint32_t handle = (uint32_t)handles()->alloc();
        DM_CHECK(handle < max(), "SparseArrayImpl::addNew() | %d, %d", handle, max());

        ObjTy* dst = &objects()[handle];
        dst = ::new (dst) ObjTy();
        return dst;
    }

    uint32_t addCopy(const ObjTy* _obj)
    {
        const uint32_t handle = (uint32_t)handles()->alloc();
        DM_CHECK(handle < max(), "SparseArrayImpl::addCopy() | %d, %d", handle, max());

        ObjTy* dst = &objects()[handle];
        dst = ::new (dst) ObjTy(*_obj);
        return handle;
    }

    bool contains(const ObjTy* _obj)
    {
        return (&objects()[0] <= _obj && _obj < &objects()[max()]);
    }

    uint32_t getHandleOf(const ObjTy* _obj)
    {
        DM_CHECK(contains(_obj), "SparseArrayImpl::getHandleOf() | Object not from the list.");

        return uint32_t(_obj - objects());
    }

    private:
    ObjTy* getObjAt_impl(uint32_t _handle)
    {
        DM_CHECK(_handle < max(), "SparseArrayImpl::getObjAt_impl() | %d, %d", _handle, max());

        return &objects()[_handle];
    } public:

    ObjTy& operator[](uint32_t _handle)
    {
        return *getObjAt_impl(_handle);
    }

    const ObjTy& operator[](uint32_t _handle) const
    {
        return *getObjAt_impl(_handle);
    }

    ObjTy* get(uint32_t _handle)
    {
        return getObjAt_impl(_handle);
    }

    /// Used for iteration over all elements.
    ObjTy* getFromHandleAt(uint32_t _idx)
    {
        const uint32_t handle = handles()->getHandleAt(_idx);
        return getObjAt_impl(handle);
    }

    void remove(uint32_t _handle)
    {
        DM_CHECK(_handle < max(), "SparseArrayImpl::removeAt() | %d, %d", _handle, max());

        objects()[_handle].~ObjTy();
        handles()->free(_handle);
    }

    void removeFromHandleAt(uint32_t _idx)
    {
        const uint32_t handle = handles()->getHandleAt(_idx);
        remove(handle);
    }

    void removeObj(const ObjTy* _obj)
    {
        const uint32_t handle = getHandleOf(_obj);
        remove(handle);
    }

    void removeAll()
    {
        for (uint32_t ii = count(); ii--; )
        {
            const uint32_t handle = handles()->getHandleAt(ii);
            remove(handle);
        }
    }

    void compact()
    {
        const uint32_t end = count();
        if (end <= 1)
        {
            return;
        }

        handles()->sort();

        uint32_t idx = 0;
        uint32_t prev;
        uint32_t curr = handles()->getHandleAt(0);

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
                    const uint32_t next = handles()->getHandleAt(cnt);
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

            curr = handles()->getHandleAt(idx);
            const uint32_t diff = curr - prev;
            inOrder = (1 == diff);
        }

        // Reset handle alloc.
        typename HandleAllocTy::HandleTy* han = handles()->handles();
        typename HandleAllocTy::HandleTy* ind = handles()->indices();
        for (uint32_t ii = 0, iiEnd = handles()->max(); ii < iiEnd; ++ii) { han[ii] = ii; }
        for (uint32_t ii = 0; ii < end; ++ii)                             { ind[ii] = ii; }
    }

    void zero()
    {
        memset(objects(), 0, max()*sizeof(ObjTy));
    }

    uint32_t count()
    {
        return handles()->count();
    }
};

template <typename ObjTy, uint32_t MaxT>
struct SparseArrayStorageT
{
    typedef ObjTy ObjectType;
    typedef HandleAllocT<MaxT> HandleAllocType;

    ObjTy* objects()
    {
        return m_objects;
    }

    HandleAllocType* handles()
    {
        return &m_handles;
    }

    uint32_t max()
    {
        return MaxT;
    }

    HandleAllocType m_handles;
    ObjTy m_objects[MaxT];
};

template <typename ObjTy>
struct SparseArrayStorageExt
{
    typedef ObjTy ObjectType;
    typedef HandleAllocExt<uint32_t> HandleAllocType;

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(ObjTy) + HandleAllocType::sizeFor(_max);
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
        uint8_t* end = m_handles.init(_max, handleBegin);

        return end;
    }

    ObjTy* objects()
    {
        return m_objects;
    }

    HandleAllocType* handles()
    {
        return &m_handles;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint32_t m_max;
    ObjTy* m_objects;
    HandleAllocType m_handles;
};

template <typename ObjTy>
struct SparseArrayStorage
{
    typedef ObjTy ObjectType;
    typedef HandleAllocExt<uint32_t> HandleAllocType;

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(ObjTy) + HandleAllocType::sizeFor(_max);
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
        m_handles.init(_max, handleBegin);

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

    HandleAllocType* handles()
    {
        return &m_handles;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint32_t m_max;
    ObjTy* m_objects;
    HandleAllocType m_handles;
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
