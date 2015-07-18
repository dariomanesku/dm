/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

template <typename PtrTy>
ValTy* insertNew(const PtrTy* _key)
{
    const HandleType handle = m_handleAlloc.alloc();
    m_hashMap.insert(_key, handle);
    return getValueOf(handle);
}

template <typename Ty>
ValTy* insertNew(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return insertNew((const void*)&_key);
}

template <typename PtrTy>
uint32_t insertObj(const PtrTy* _key, const ValTy& _obj)
{
    const HandleType handle = m_handleAlloc.alloc();
    m_hashMap.insert(_key, handle);
    ValTy* dst = getValueOf(handle);
    dst = ::new (dst) ValTy(_obj);
    return handle;
}

template <typename Ty>
ValTy* insertObj(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return insertObj((const void*)&_key);
}

template <typename PtrTy>
uint32_t unsafeFindHandleOf(const PtrTy* _key)
{
    return m_hashMap.unsafeFindHandleOf(_key);
}

template <typename Ty>
uint32_t unsafeFindHandleOf(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return unsafeFindHandleOf((const void*)&_key);
}

ValTy* getValueOf(HandleType _handle)
{
    return &m_objects[_handle];
}

template <typename PtrTy>
ValTy* unsafeFind(const PtrTy* _key)
{
    return getValueOf(unsafeFindHandleOf(_key));
}

template <typename Ty>
ValTy* unsafeFind(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return unsafeFind((const void*)&_key);
}

template <typename PtrTy>
uint32_t findIdxOf(const PtrTy* _key, uint32_t _lookAhead = UINT32_MAX)
{
    return m_hashMap.findIdxOf(_key, _lookAhead);
}

template <typename Ty>
uint32_t findIdxOf(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return findIdxOf((const void*)&_key, _lookAhead);
}

template <typename PtrTy>
ValTy* find(const PtrTy* _key, uint32_t _lookAhead = UINT32_MAX)
{
    const uint32_t idx = m_hashMap.findIdxOf(_key, _lookAhead);
    if (InvalidIdx != idx)
    {
        const HandleType handle = { idx };
        return getValueOf(handle);
    }
    else
    {
        return NULL;
    }
}

template <typename Ty>
ValTy* find(const Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return find((const void*)&_key, _lookAhead);
}

template <typename PtrTy>
bool contains(const PtrTy* _key, uint32_t _lookAhead = UINT32_MAX)
{
    return (InvalidIdx != findIdxOf(_key, _lookAhead));
}

template <typename Ty>
bool contains(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    dm_staticAssert(is_arithmetic<Ty>::value);

    return contains((const void*)&_key, _lookAhead);
}

template <typename PtrTy>
void unsafeRemove(const PtrTy* _key)
{
    const uint32_t idx = m_hashMap.unsafeRemove(_key);
    const HandleType handle = { idx };
    m_handleAlloc.free(handle);
    ValTy* dst = getValueOf(handle);
    dst->~ValTy();
}

template <typename Ty>
void unsafeRemove(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    unsafeRemove((const void*)&_key);
}

template <typename PtrTy>
bool remove(const PtrTy* _key, uint32_t _lookAhead = UINT32_MAX)
{
    const HandleType handle = m_hashMap.find(_key, _lookAhead);
    if (InvalidIdx != handle)
    {
        m_handleAlloc.free(handle);
        ValTy* dst = getValueOf(handle);
        dst->~ValTy();
        return true;
    }
    else
    {
        return false;
    }
}

template <typename Ty>
bool remove(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return remove((const void*)&_key, _lookAhead);
}

uint32_t count() const
{
    return (uint32_t)m_handleAlloc.count();
}

ValTy* getValueAt(uint32_t _idx)
{
    const HandleType handle = { m_handleAlloc.getHandleAt(_idx) };
    return getValueOf(handle);
}

/* vim: set sw=4 ts=4 expandtab: */
