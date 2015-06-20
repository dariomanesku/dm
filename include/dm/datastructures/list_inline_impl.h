/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

static typename HandleAlloc::HandleType invalid()
{
    return HandleAlloc::invalid();
}

void fillWith(const ObjTy* _obj)
{
    ObjTy* elem = m_elements;
    for (uint16_t ii = count(); ii--; )
    {
        ::new (&elem[ii]) ObjTy(*_obj);
    }
}

uint16_t add(const ObjTy& _obj)
{
    const uint16_t idx = m_handles.alloc();

    ObjTy* dst = &m_elements[idx];
    dst = ::new (dst) ObjTy(_obj);
    return idx;
}

ObjTy* addNew()
{
    const uint16_t idx = m_handles.alloc();
    DM_CHECK(idx < max(), "listAddNew | %d, %d", idx, max());

    ObjTy* dst = &m_elements[idx];
    dst = ::new (dst) ObjTy();
    return dst;
}

bool contains(uint16_t _handle)
{
    return m_handles.contains(_handle);
}

bool containsObj(const ObjTy* _obj) const
{
    return (&m_elements[0] <= _obj && _obj < &m_elements[max()]);
}

uint16_t getHandleOf(const ObjTy* _obj) const
{
    DM_CHECK(containsObj(_obj), "listGetHandleOf | Object not from the list.");

    return uint16_t(_obj - m_elements);
}

uint16_t getHandleAt(uint16_t _idx) const
{
    return m_handles.getHandleAt(_idx);
}

ObjTy* getObjFromHandle(uint16_t _handle)
{
    DM_CHECK(_handle < max(), "listGetObjFromHandle | %d, %d", _handle, max());

    return const_cast<ObjTy*>(&m_elements[_handle]);
}

private:
ObjTy* getObjAt_impl(uint16_t _idx)
{
    DM_CHECK(_idx < max(), "listGetObjAt | %d, %d", _idx, max());

    const uint16_t handle = m_handles.getHandleAt(_idx);
    return this->getObjFromHandle(handle);
}
public:

ObjTy* getObjAt(uint16_t _idx)
{
    This* list = const_cast<This*>(this);
    return list->getObjAt_impl(_idx);
}

const ObjTy* getObjAt(uint16_t _idx) const
{
    This* list = const_cast<This*>(this);
    return list->getObjAt_impl(_idx);
}

ObjTy& operator[](uint16_t _idx)
{
    This* list = const_cast<This*>(this);
    return *list->getObjAt_impl(_idx);
}

const ObjTy& operator[](uint16_t _idx) const
{
    This* list = const_cast<This*>(this);
    return *list->getObjAt_impl(_idx);
}

void remove(uint16_t _handle)
{
    DM_CHECK(_handle < max(), "listRemove | %d, %d", _handle, max());

    m_elements[_handle].~ObjTy();
    m_handles.free(_handle);
}

uint16_t removeObj(ObjTy* _obj)
{
    const uint16_t handle = getHandleOf(_obj);
    this->remove(handle);
    return handle;
}

void removeAt(uint16_t _idx)
{
    DM_CHECK(_idx < max(), "listRemoveAt | %d, %d", _idx, max());

    const uint16_t handle = m_handles.getHandleAt(_idx);
    this->remove(handle);
}

void removeAll()
{
    for (uint16_t ii = count(); ii--; )
    {
        this->getObjAt(ii)->~ObjTy();
    }
    m_handles.reset();
}

void reset()
{
    m_handles.reset();
}

/* vim: set sw=4 ts=4 expandtab: */
