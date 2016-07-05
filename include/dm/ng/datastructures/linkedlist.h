/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_LINKEDLIST_HEADER_GUARD
#define DM_NG_LINKEDLIST_HEADER_GUARD

#include "handlealloc.h"
#include <dm/check.h>
#include <dm/ng/allocator.h>

#ifndef DM_DEBUG_LINKEDLIST
#   define DM_DEBUG_LINKEDLIST 0
#endif // DM_DEBUG_LINKEDLIST

#if DM_DEBUG_LINKEDLIST
#   include <stdio.h>
#   include <dm/ng/debug.h>
#endif //DM_DEBUG_LINKEDLIST

namespace dm { namespace ng {

template <typename LinkedListStorageTy>
struct LinkedListImpl : LinkedListStorageTy
{
    /// Expected interface:
    ///
    /// template <typename ObjTy>
    /// struct LinkedListStorageT
    /// {
    ///     typedef ObjTy ObjectType;
    ///     typedef HandleAllocT<MaxT> HandleAllocType;
    ///
    ///     struct Node
    ///     {
    ///         uint16_t m_prev;
    ///         uint16_t m_next;
    ///     };
    ///     struct Elem : Node, ObjTy { };
    ///
    ///     Elem* elements();
    ///     HandleAllocType* handles();
    ///     uint16_t max();
    /// };
    typedef typename LinkedListStorageTy::ObjectType      ObjTy;
    typedef typename LinkedListStorageTy::HandleAllocType HandleAllocTy;
    typedef typename LinkedListStorageTy::Node            NodeTy;
    typedef typename LinkedListStorageTy::Elem            ElemTy;
    using LinkedListStorageTy::elements;
    using LinkedListStorageTy::handles;
    using LinkedListStorageTy::max;

    LinkedListImpl() : LinkedListStorageTy()
    {
    }

    void init()
    {
        elements()[0].m_prev = 0;
        elements()[0].m_next = 0;
        m_last = 0;
    }

    ObjTy* addNew()
    {
        ObjTy* elem = insertAfter(m_last);
        elem = ::new (elem) ObjTy();

        return elem;
    }

    ObjTy* insertAfter(uint16_t _handle)
    {
        DM_CHECK(_handle < max(), "LinkedListImpl::insertAfter() | %d, %d", _handle, max());

        ElemTy* elem = (ElemTy*)getObj(_handle);
        return insertAfter(elem);
    }

    ObjTy* insertAfter(const ObjTy* _obj)
    {
        const uint16_t idx = handles()->alloc();

        ElemTy* elem = &elements()[idx];
        elem = ::new (elem) ElemTy();

        ElemTy* prev = (ElemTy*)_obj;
        const uint16_t prevHandle = getHandle(prev);
        ElemTy* next = (ElemTy*)getObj(prev->m_next);

        elem->m_prev = prevHandle;
        elem->m_next = prev->m_next;

        prev->m_next = idx;
        next->m_prev = idx;

        if (m_last == prevHandle)
        {
            m_last = idx;
        }

        checkList();

        return elem;
    }

    ObjTy* next(const ObjTy* _obj)
    {
        DM_CHECK(contains(_obj), "LinkedListImpl::next() | Object not from the list.");

        const ElemTy* elem = (const ElemTy*)_obj;
        return &elements()[elem->m_next];
    }

    ObjTy* prev(const ObjTy* _obj)
    {
        DM_CHECK(contains(_obj), "LinkedListImpl::prev() | Object not from the list.");

        const ElemTy* elem = (const ElemTy*)_obj;
        return &elements()[elem->m_prev];
    }

    uint16_t next(uint16_t _handle)
    {
        DM_CHECK(_handle < max(), "LinkedListImpl::next() | %d, %d", _handle, max());

        ElemTy* elem = (ElemTy*)getObj(_handle);
        return elem->m_next;
    }

    uint16_t prev(uint16_t _handle)
    {
        DM_CHECK(_handle < max(), "LinkedListImpl::prev() | %d, %d", _handle, max());

        ElemTy* elem = (ElemTy*)getObj(_handle);
        return elem->m_prev;
    }

    ObjTy* lastElem()
    {
        return getObj(m_last);
    }

    ObjTy* firstElem()
    {
        ElemTy* last = ((ElemTy*)getObj(m_last));
        return getObj(last->m_next);
    }

    uint16_t lastHandle()
    {
        return m_last;
    }

    uint16_t firstHandle()
    {
        ElemTy* elem = (ElemTy*)getObj(m_last);
        return elem->m_next;
    }

    uint16_t getHandle(const ObjTy* _obj)
    {
        DM_CHECK(contains(_obj), "LinkedListImpl::getHandle() | Object not from the list.");

        return (uint16_t)((ElemTy*)_obj - elements());
    }

    ObjTy* getObj(uint16_t _handle)
    {
        DM_CHECK(_handle < max(), "LinkedListImpl::getObj() | %d, %d", _handle, max());

        return &elements()[_handle];
    }

    private: ObjTy* getObjAt_impl(uint16_t _idx)
    {
        DM_CHECK(_idx < max(), "LinkedListImpl::getObjAt() | %d, %d", _idx, max());

        const uint16_t handle = handles()->getHandleAt(_idx);
        return &elements()[handle];
    } public:

    ObjTy* getObjAt(uint16_t _idx)
    {
        return getObjAt_impl(_idx);
    }

    ObjTy* operator[](uint16_t _idx)
    {
        return getObjAt_impl(_idx);
    }

    const ObjTy* operator[](uint16_t _idx) const
    {
        return getObjAt_impl(_idx);
    }

    void remove(uint16_t _handle)
    {
        DM_CHECK(_handle < max(), "LinkedListImpl::remove() | %d, %d", _handle, max());

        ElemTy* elem = (ElemTy*)getObj(_handle);
        ElemTy* prev = (ElemTy*)getObj(elem->m_prev);
        ElemTy* next = (ElemTy*)getObj(elem->m_next);

        prev->m_next = elem->m_next;
        next->m_prev = elem->m_prev;

        handles()->free(_handle);

        if (_handle == m_last)
        {
            m_last = elem->m_prev;
        }

        checkList();
    }

    void removeAll()
    {
        for (uint16_t ii = handles()->count(); ii--; )
        {
            ElemTy* elem = getObj(handles()->getHandleAt(ii));
            elem->~ElemTy();
            BX_UNUSED(elem);
        }

        reset();
    }

    void reset()
    {
        handles()->reset();
        elements()[0].m_prev = 0;
        elements()[0].m_next = 0;
        m_last = 0;
    }

    bool contains(uint16_t _handle)
    {
        return handles()->contains(_handle);
    }

    bool contains(const ObjTy* _obj)
    {
        return (&elements()[0] <= _obj && _obj < &elements()[max()]);
    }

    uint16_t count()
    {
        return handles()->count();
    }

    #if DM_DEBUG_LINKEDLIST
    void checkList()
    {
        ElemTy* begin = (ElemTy*)firstElem();
        ElemTy* end   = (ElemTy*)lastElem();

        printf("L |");
        ElemTy* curr = begin;
        for (uint16_t ii = count()-1; ii--; )
        {
            printf("%d %d %d|", curr->m_prev, getHandle(curr), curr->m_next);
            curr = (ElemTy*)next(curr);
        }
        printf("%d %d %d|\n", curr->m_prev, getHandle(curr), curr->m_next);

        if (curr != end)
        {
            debugBreak();
        }
    }
    #else
    void checkList()
    {
    }
    #endif // DM_DEBUG_LINKEDLIST

private:
    uint16_t m_last;
};

template <typename ObjTy, uint16_t MaxT>
struct LinkedListStorageT
{
    typedef ObjTy ObjectType;
    typedef HandleAllocT<MaxT> HandleAllocType;

    struct Node
    {
        uint16_t m_prev;
        uint16_t m_next;
    };

    struct Elem : Node, ObjTy
    {
    };

    Elem* elements()
    {
        return m_elements;
    }

    HandleAllocType* handles()
    {
        return &m_handles;
    }

    uint16_t max()
    {
        return MaxT;
    }

    HandleAllocType m_handles;
    Elem m_elements[MaxT];
};

template <typename ObjTy>
struct LinkedListStorageExt
{
    typedef ObjTy ObjectType;
    typedef HandleAllocExt<uint16_t> HandleAllocType;

    struct Node
    {
        uint16_t m_prev;
        uint16_t m_next;
    };

    struct Elem : Node, ObjTy
    {
    };

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(Elem) + HandleAllocType::sizeFor(_max);
    }

    LinkedListStorageExt()
    {
        m_max = 0;
        m_elements = NULL;
    }

    uint8_t* initStorage(uint32_t _max, uint8_t* _mem)
    {
        uint8_t* elemBegin   = (uint8_t*)_mem;
        uint8_t* handleBegin = (uint8_t*)_mem + _max*sizeof(Elem);

        m_max = _max;
        m_elements = (Elem*)elemBegin;
        uint8_t* end = m_handles.init(_max, handleBegin);

        return end;
    }

    Elem* elements()
    {
        return m_elements;
    }

    HandleAllocType* handles()
    {
        return &m_handles;
    }

    uint16_t max()
    {
        return m_max;
    }

    uint16_t m_max;
    Elem* m_elements;
    HandleAllocType m_handles;
};

template <typename ObjTy>
struct LinkedListStorage
{
    typedef ObjTy ObjectType;
    typedef HandleAllocExt<uint16_t> HandleAllocType;

    struct Node
    {
        uint16_t m_prev;
        uint16_t m_next;
    };

    struct Elem : Node, ObjTy
    {
    };

    static uint32_t sizeFor(uint32_t _max)
    {
        return _max*sizeof(Elem) + HandleAllocType::sizeFor(_max);
    }

    LinkedListStorage()
    {
        m_max = 0;
        m_elements = NULL;
    }

    void initStorage(uint32_t _max, ReallocFn _reallocFn = ::realloc)
    {
        const uint32_t totalSize = sizeFor(_max);
        void* mem = dm_alloc(totalSize, _reallocFn);

        uint8_t* elemBegin   = (uint8_t*)mem;
        uint8_t* handleBegin = (uint8_t*)mem + _max*sizeof(Elem);

        m_max = _max;
        m_elements = (Elem*)elemBegin;
        m_handles.init(_max, handleBegin);

        m_reallocFn = _reallocFn;
    }

    void destroy()
    {
        if (NULL != m_elements)
        {
            dm_free(m_elements, m_reallocFn);
            m_elements = NULL;
        }
    }

    Elem* elements()
    {
        return m_elements;
    }

    HandleAllocType* handles()
    {
        return &m_handles;
    }

    uint16_t max()
    {
        return m_max;
    }

    uint16_t m_max;
    Elem* m_elements;
    HandleAllocType m_handles;
    ReallocFn m_reallocFn;
};

template <typename ObjTy, uint16_t MaxT>
struct LinkedListT : LinkedListImpl< LinkedListStorageT<ObjTy, MaxT> >
{
    typedef LinkedListImpl< LinkedListStorageT<ObjTy, MaxT> > Base;

    LinkedListT() : Base()
    {
        Base::init();
    }
};

template <typename ObjTy>
struct LinkedListExt : LinkedListImpl< LinkedListStorageExt<ObjTy> >
{
    typedef LinkedListImpl< LinkedListStorageExt<ObjTy> > Base;

    uint8_t* init(uint32_t _max, uint8_t* _mem)
    {
        uint8_t* ptr = Base::initStorage(_max, _mem);
        Base::init();

        return ptr;
    }
};

template <typename ObjTy>
struct LinkedList : LinkedListImpl< LinkedListStorage<ObjTy> >
{
    typedef LinkedListImpl< LinkedListStorage<ObjTy> > Base;

    void init(uint32_t _max, ReallocFn _reallocFn = ::realloc)
    {
        Base::initStorage(_max, _reallocFn);
        Base::init();
    }
};

template <typename ObjTy>
struct LinkedListH : LinkedListExt<ObjTy>
{
    ReallocFn m_reallocFn;
};

} //namespace ng
} //namespace dm

#endif // DM_NG_LINKEDLIST_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
