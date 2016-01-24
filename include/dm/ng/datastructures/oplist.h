/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_OPLIST_HEADER_GUARD
#define DM_NG_OPLIST_HEADER_GUARD

#include <new>
#include <dm/check.h>
#include <dm/ng/allocator.h>
#include "handlealloc.h"

namespace dm { namespace ng {

template <typename OpListStorage>
struct OpListImpl : OpListStorage
{
    /// Expected interface:
    ///
    ///     template <typename ObjTy>
    ///     struct OpListStorageTemplate
    ///     {
    ///         typedef ArrayT<uint16_t, MaxT> ArrayType;
    ///         typedef HandleAllocT<MaxT>     HandleAllocType;
    ///         typedef ObjTy                  ObjectType;
    ///
    ///         ArrayType* handles();
    ///         HandleAllocType* handleAlloc();
    ///         ObjectType* objects();
    ///         uint16_t max();
    ///     };

    typedef typename OpListStorage::ArrayType       ArrayTy;
    typedef typename OpListStorage::HandleAllocType HandleAllocTy;
    typedef typename OpListStorage::ObjectType      ObjTy;
    using OpListStorage::handles;
    using OpListStorage::handleAlloc;
    using OpListStorage::objects;
    using OpListStorage::max;

    OpListImpl() : OpListStorage()
    {
    }

    ObjTy* addNew()
    {
        const uint16_t handle = handleAlloc()->alloc();
        handles()->push(handle);

        ObjTy* obj = &objects()[handle];
        obj = ::new (obj) ObjTy();

        return obj;
    }

    uint16_t addCopy(const ObjTy* _obj)
    {
        const uint16_t handle = handleAlloc()->alloc();
        handles()->push(handle);

        ObjTy* obj = &objects()[handle];
        obj = ::new (obj) ObjTy(*_obj);

        return handle;
    }

    void removeAt(uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "OpList::removeAt() | %d, %d", _idx, max());

        const uint16_t handle = handles()->get(_idx);
        handles()->remove(_idx);

        handleAlloc()->free(handle);
        objects()[handle].~ObjTy();
    }

    void removeAll()
    {
        for (uint16_t ii = count(); ii--; )
        {
            objects()[ii].~ObjTy();
        }
        handleAlloc()->reset();
        handles()->reset();
    }

    ObjTy* getAt(uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "OpList::getAt() | %d, %d", _idx, max());

        const uint16_t handle = handles()->get(_idx);
        return &objects()[handle];
    }

    ObjTy& operator[](uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "OpList::[]() | %d, %d", _idx, max());

        const uint16_t handle = handles()->get(_idx);
        return objects()[handle];
    }

    const ObjTy& operator[](uint16_t _idx) const
    {
        DM_CHECK(_idx < max(), "OpList::[]() const | %d, %d", _idx, max());

        const uint16_t handle = handles()->get(_idx);
        return objects()[handle];
    }

    uint16_t getHandleAt(uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "OpList::getHandleAt() | %d, %d", _idx, max());

        return handles()->get(_idx);
    }

    uint16_t getHandleOf(const ObjTy* _obj)
    {
        DM_CHECK(&objects()[0] <= _obj && _obj < &objects()[max()], "OpList::getHandleOf() | Object not from the list.");

        return uint16_t(_obj - objects());
    }

    ObjTy* getFromHandle(uint16_t _handle)
    {
        DM_CHECK(_handle < max(), "OpList::getFromHandle() | %d, %d", _handle, max());

        return &objects()[_handle];
    }

    bool contains(uint16_t _handle)
    {
        return handleAlloc()->contains(_handle);
    }

    bool containsObj(const ObjTy* _obj)
    {
        return (&objects()[0] <= _obj && _obj < &objects()[max()]);
    }

    uint16_t count()
    {
        return handleAlloc()->count();
    }
};

template <typename ObjTy, uint16_t MaxT>
struct OpListStorageT
{
    typedef ArrayT<uint16_t, MaxT> ArrayType;
    typedef HandleAllocT<MaxT>     HandleAllocType;
    typedef ObjTy                  ObjectType;

    ArrayType* handles()
    {
        return &m_handles;
    }

    HandleAllocType* handleAlloc()
    {
        return &m_handleAlloc;
    }

    ObjectType* objects()
    {
        return m_objects;
    }

    uint16_t max()
    {
        return MaxT;
    }

    ArrayType m_handles;
    HandleAllocType m_handleAlloc;
    ObjTy m_objects[MaxT];
};

template <typename ObjTy>
struct OpListStorageExt
{
    typedef ArrayExt<uint16_t>       ArrayType;
    typedef HandleAllocExt<uint16_t> HandleAllocType;
    typedef ObjTy                    ObjectType;

    static uint32_t sizeFor(uint16_t _max)
    {
        return ArrayType::sizeFor(_max) + HandleAllocType::sizeFor(_max) + _max*sizeof(ObjTy);
    }

    OpListStorageExt()
    {
        m_max = 0;
        m_objects = NULL;
    }

    uint8_t* init(uint16_t _max, uint8_t* _mem)
    {
        uint8_t* ptr = _mem;
        m_objects = (ObjTy*)ptr;
        ptr += _max*sizeof(ObjTy);
        ptr = m_handles.init(_max, ptr + _max*sizeof(ObjTy));
        ptr = m_handleAlloc.init(_max, ptr);
        m_max = _max;

        return ptr;
    }

    ArrayType* handles()
    {
        return &m_handles;
    }

    HandleAllocType* handleAlloc()
    {
        return &m_handleAlloc;
    }

    ObjectType* objects()
    {
        return m_objects;
    }

    uint16_t max()
    {
        return m_max;
    }

    ArrayType m_handles;
    HandleAllocType m_handleAlloc;
    ObjTy* m_objects;
    uint16_t m_max;
};

template <typename ObjTy>
struct OpListStorage
{
    typedef ArrayExt<uint16_t>       ArrayType;
    typedef HandleAllocExt<uint16_t> HandleAllocType;
    typedef ObjTy                    ObjectType;

    static uint32_t sizeFor(uint16_t _max)
    {
        return ArrayType::sizeFor(_max) + HandleAllocType::sizeFor(_max) + _max*sizeof(ObjTy);
    }

    OpListStorage()
    {
        m_max = 0;
        m_objects = NULL;
    }

    void init(uint16_t _max, Allocator* _allocator)
    {
        const uint32_t totalSize = sizeFor(_max);
        void* mem = _allocator->m_allocFunc(totalSize);

        uint8_t* ptr = (uint8_t*)mem;
        m_objects = (ObjTy*)ptr;
        ptr += _max*sizeof(ObjTy);
        ptr = m_handles.init(_max, ptr + _max*sizeof(ObjTy));
        ptr = m_handleAlloc.init(_max, ptr);
        m_max = _max;

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

    ArrayType* handles()
    {
        return &m_handles;
    }

    HandleAllocType* handleAlloc()
    {
        return &m_handleAlloc;
    }

    ObjectType* objects()
    {
        return m_objects;
    }

    uint16_t max()
    {
        return m_max;
    }

    ArrayType m_handles;
    HandleAllocType m_handleAlloc;
    ObjTy* m_objects;
    uint16_t m_max;
    FreeFunc m_freeFunc;
};

template <typename ObjTy, uint32_t MaxT> struct OpListT   : OpListImpl< OpListStorageT<ObjTy, MaxT> > { };
template <typename ObjTy>                struct OpListExt : OpListImpl< OpListStorageExt<ObjTy>     > { };
template <typename ObjTy>                struct OpList    : OpListImpl< OpListStorage<ObjTy>        > { };
template <typename ObjTy>                struct OpListH   : OpListExt<ObjTy> { FreeFunc m_freeFunc; };

} //namespace ng
} //namespace dm

#endif // DM_NG_OPLIST_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
