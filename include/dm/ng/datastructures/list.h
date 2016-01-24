/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_LIST_HEADER_GUARD
#define DM_NG_LIST_HEADER_GUARD

#include <new>
#include <dm/check.h>
#include <dm/ng/allocator.h>
#include "handlealloc.h"

namespace dm { namespace ng {

template <typename ListStorage>
struct ListImpl : ListStorage
{
    /// Expected interface:
    ///
    ///     template <typename ObjTy>
    ///     struct ListStorageT
    ///     {
    ///         typedef ObjTy ObjectType;
    ///         typedef HandleAllocT<MaxT> HandleAllocType;
    ///
    ///         ObjTy* objects();
    ///         HandleAllocType* handles();
    ///         uint32_t max();
    ///     };
    typedef typename ListStorage::ObjectType      ObjTy;
    typedef typename ListStorage::HandleAllocType HandleAllocTy;
    using ListStorage::objects;
    using ListStorage::handles;
    using ListStorage::max;

    ListImpl() : ListStorage()
    {
    }

    void fillWith(const ObjTy* _obj)
    {
        for (uint32_t ii = count(); ii--; )
        {
            ::new (&objects()[ii]) ObjTy(*_obj);
        }
    }

    uint32_t addCopy(const ObjTy* _obj)
    {
        const uint32_t idx = handles()->alloc();

        ObjTy* dst = &objects()[idx];
        dst = ::new (dst) ObjTy(*_obj);
        return idx;
    }

    ObjTy* addNew()
    {
        const uint32_t idx = handles()->alloc();
        DM_CHECK(idx < max(), "ListImpl::addNew() | %d, %d", idx, max());

        ObjTy* dst = &objects()[idx];
        dst = ::new (dst) ObjTy();
        return dst;
    }

    bool contains(uint32_t _handle)
    {
        return handles()->contains(_handle);
    }

    bool containsObj(const ObjTy* _obj)
    {
        return (&objects()[0] <= _obj && _obj < &objects()[max()]);
    }

    uint32_t getHandleOf(const ObjTy* _obj)
    {
        DM_CHECK(containsObj(_obj), "ListImpl::getHandleOf() | Object not from the list.");

        return uint32_t(_obj - objects());
    }

    uint32_t getHandleAt(uint32_t _idx)
    {
        return handles()->getHandleAt(_idx);
    }

    ObjTy* get(uint32_t _handle)
    {
        DM_CHECK(_handle < max(), "ListImpl::getObjFromHandle() | %d, %d", _handle, max());

        return &objects()[_handle];
    }

    private:
    ObjTy* getObjAt_impl(uint32_t _idx)
    {
        DM_CHECK(_idx < max(), "ListImpl::getObjAt_impl() | %d, %d", _idx, max());

        const uint32_t handle = handles()->getHandleAt(_idx);
        return get(handle);
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

    void remove(uint32_t _handle)
    {
        DM_CHECK(_handle < max(), "ListImpl::remove() | %d, %d", _handle, max());

        objects()[_handle].~ObjTy();
        handles()->free(_handle);
    }

    uint32_t removeObj(ObjTy* _obj)
    {
        const uint32_t handle = getHandleOf(_obj);
        remove(handle);
        return handle;
    }

    void removeAt(uint32_t _idx)
    {
        DM_CHECK(_idx < max(), "ListImpl::removeAt() | %d, %d", _idx, max());

        const uint32_t handle = handles()->getHandleAt(_idx);
        remove(handle);
    }

    void removeAll()
    {
        for (uint32_t ii = count(); ii--; )
        {
            getAt(ii)->~ObjTy();
        }
        handles()->reset();
    }

    void reset()
    {
        handles()->reset();
    }

    uint32_t count()
    {
        return handles()->count();
    }

private:
    uint32_t m_last;
};

template <typename ObjTy, uint32_t MaxT>
struct ListStorageT
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
struct ListStorageExt
{
    typedef ObjTy ObjectType;
    typedef HandleAllocExt<uint32_t> HandleAllocType;

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(ObjTy) + HandleAllocType::sizeFor(_max);
    }

    ListStorageExt()
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
struct ListStorage
{
    typedef ObjTy ObjectType;
    typedef HandleAllocExt<uint32_t> HandleAllocType;

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(ObjTy) + HandleAllocType::sizeFor(_max);
    }

    ListStorage()
    {
        m_max = 0;
        m_objects = NULL;
    }

    void init(uint32_t _max, Allocator* _allocator)
    {
        const uint32_t totalSize = sizeFor(_max);
        void* mem = _allocator->m_allocFunc(totalSize);

        uint8_t* objBegin   = (uint8_t*)mem;
        uint8_t* handleBegin = (uint8_t*)mem + _max*sizeof(ObjTy);

        m_max = _max;
        m_objects = (ObjTy*)objBegin;
        m_handles.init(_max, handleBegin);

        m_freeFunc = _allocator->m_freeFunc;
    }

    void destroy()
    {
        if (NULL != m_objects)
        {
            m_freeFunc(m_objects);
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
    FreeFunc m_freeFunc;
};

template <typename ObjTy, uint32_t MaxT> struct ListT   : ListImpl< ListStorageT<ObjTy, MaxT> > { };
template <typename ObjTy>                struct ListExt : ListImpl< ListStorageExt<ObjTy>     > { };
template <typename ObjTy>                struct List    : ListImpl< ListStorage<ObjTy>        > { };
template <typename ObjTy>                struct ListH   : ListExt<ObjTy> { FreeFunc m_freeFunc; };

} //namespace ng
} //namespace dm

#endif // DM_NG_LIST_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
