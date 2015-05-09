/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

struct Node
{
    uint16_t m_prev;
    uint16_t m_next;
};

struct Elem : Node,Ty
{
};

Ty* insertAfter(const Ty* _obj)
{
    const uint16_t idx = m_handles.alloc();

    Elem* elem = &m_elements[idx];
    elem = ::new (elem) Elem();

    Elem* prev = (Elem*)_obj;
    const uint16_t prevHandle = getHandle(prev);
    Elem* next = (Elem*)getObj(prev->m_next);

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

Ty* addNew()
{
    return insertAfter(m_last);
}

Ty* insertAfter(uint16_t _handle)
{
    DM_CHECK(_handle < max(), "LinkedListT::insertAfter | %d, %d", _handle, max());

    Elem* elem = (Elem*)getObj(_handle);
    return insertAfter(elem);
}

Ty* next(const Ty* _obj)
{
    DM_CHECK(contains(_obj), "LinkedListT::next | Object not from the list.");

    const Elem* elem = (const Elem*)_obj;
    return &m_elements[elem->m_next];
}

Ty* prev(const Ty* _obj)
{
    DM_CHECK(contains(_obj), "LinkedListT::prev | Object not from the list.");

    const Elem* elem = (const Elem*)_obj;
    return &m_elements[elem->m_prev];
}

uint16_t next(uint16_t _handle)
{
    DM_CHECK(_handle < max(), "llNext | %d, %d", _handle, max());

    Elem* elem = (Elem*)getObj(_handle);
    return elem->m_next;
}

uint16_t prev(uint16_t _handle)
{
    DM_CHECK(_handle < max(), "llPrev | %d, %d", _handle, max());

    Elem* elem = (Elem*)getObj(_handle);
    return elem->m_prev;
}

Ty* lastElem()
{
    return getObj(m_last);
}

Ty* firstElem()
{
    Elem* last = ((Elem*)getObj(m_last));
    return getObj(last->m_next);
}

uint16_t lastHandle()
{
    return m_last;
}

uint16_t firstHandle()
{
    Elem* elem = (Elem*)getObj(m_last);
    return elem->m_next;
}

uint16_t getHandle(const Ty* _obj)
{
    DM_CHECK(contains(_obj), "llGetHandle | Object not from the list.");

    return (uint16_t)((Elem*)_obj - m_elements);
}

Ty* getObj(uint16_t _handle)
{
    DM_CHECK(_handle < max(), "llGetObj | %d, %d", _handle, max());

    return &m_elements[_handle];
}

private: Ty* getObjAt_impl(uint16_t _idx)
{
    DM_CHECK(_idx < max(), "llGetObjAt | %d, %d", _idx, max());

    const uint16_t handle = m_handles.getHandleAt(_idx);
    return &m_elements[handle];
} public:

Ty* getObjAt(uint16_t _idx)
{
    This* list = const_cast<This*>(this);
    return list->getObjAt_impl(_idx);
}

Ty* operator[](uint16_t _idx)
{
    This* list = const_cast<This*>(this);
    return list->getObjAt_impl(_idx);
}

const Ty* operator[](uint16_t _idx) const
{
    This* list = const_cast<This*>(this);
    return list->getObjAt_impl(_idx);
}

void remove(uint16_t _handle)
{
    DM_CHECK(_handle < max(), "llRemove | %d, %d", _handle, max());

    Elem* elem = (Elem*)getObj(_handle);
    Elem* prev = (Elem*)getObj(elem->m_prev);
    Elem* next = (Elem*)getObj(elem->m_next);

    prev->m_next = elem->m_next;
    next->m_prev = elem->m_prev;

    m_handles.free(_handle);

    if (_handle == m_last)
    {
        m_last = elem->m_prev;
    }

    checkList();
}

void removeAll()
{
    for (uint16_t ii = m_handles.count(); ii--; )
    {
        Elem* elem = getObj(m_handles.getHandleAt(ii));
        elem->~Elem();
        BX_UNUSED(elem);
    }

    reset();
}

void reset()
{
    m_handles.reset();
    m_elements[0].m_prev = 0;
    m_elements[0].m_next = 0;
    m_last = 0;
}

bool contains(uint16_t _handle)
{
    return m_handles.contains(_handle);
}

bool contains(const Ty* _obj)
{
    return (&m_elements[0] <= _obj && _obj < &m_elements[max()]);
}

#if 0 // Debug only !
#include <stdio.h>
#include "../../../3rdparty/bx/debug.h"

void checkList()
{
    Elem* begin = firstElem();
    Elem* end = lastElem();

    printf("L |");
    Elem* curr = begin;
    for (uint16_t ii = count()-1; ii--; )
    {
        printf("%d %d %d|", curr->m_prev, getHandle(curr), curr->m_next);
        curr = next(curr);
    }
    printf("%d %d %d|\n", curr->m_prev, getHandle(curr), curr->m_next);

    if (curr != end)
    {
        bx::debugBreak();
    }
}
#else
void checkList()
{
}
#endif //1

/* vim: set sw=4 ts=4 expandtab: */
