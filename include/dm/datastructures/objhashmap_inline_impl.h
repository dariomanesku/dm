/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

ValTy* insertNew(const void* _key)
{
    const HandleType handle = m_handleAlloc.alloc();
    m_hashMap.insert(_key, handle);
    return getValueOf(handle);
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
ValTy* insertNew(Ty _key)
{
    return insertNew((const void*)&_key);
}

uint32_t insertObj(const void* _key, const ValTy& _obj)
{
    const HandleType handle = m_handleAlloc.alloc();
    m_hashMap.insert(_key, handle);
    ValTy* dst = getValueOf(handle);
    dst = ::new (dst) ValTy(_obj);
    return handle;
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
ValTy* insertObj(Ty _key)
{
    return insertObj((const void*)&_key);
}

uint32_t unsafeFindHandleOf(const void* _key)
{
    return m_hashMap.unsafeFindHandleOf(_key);
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
uint32_t unsafeFindHandleOf(Ty _key)
{
    return unsafeFindHandleOf(_key);
}

ValTy* getValueOf(HandleType _handle)
{
    return &m_objects[_handle];
}

ValTy* unsafeFind(const void* _key)
{
    return getValueOf(unsafeFindHandleOf(_key));
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
ValTy* unsafeFind(Ty _key)
{
    return unsafeFind((const void*)&_key);
}

uint32_t findIdxOf(const void* _key, uint32_t _lookAhead = UINT32_MAX)
{
    return m_hashMap.findIdxOf(_key, _lookAhead);
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
uint32_t findIdxOf(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    return findIdxOf((const void*)&_key, _lookAhead);
}

ValTy* find(const void* _key, uint32_t _lookAhead = UINT32_MAX)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
ValTy* find(const void* _key, uint32_t _lookAhead = UINT32_MAX)
{
    return find((const void*)&_key, _lookAhead);
}

bool contains(const void* _key, uint32_t _lookAhead = UINT32_MAX)
{
    return (InvalidIdx != findIdxOf(_key, _lookAhead));
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic)>
bool contains(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    return contains((const void*)&_key, _lookAhead);
}

void unsafeRemove(const void* _key)
{
    const uint32_t idx = m_hashMap.unsafeRemove(_key);
    const HandleType handle = { idx };
    m_handleAlloc.free(handle);
    ValTy* dst = getValueOf(handle);
    dst->~ValTy();
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
void unsafeRemove(Ty _key)
{
    unsafeRemove((const void*)&_key);
}

bool remove(const void* _key, uint32_t _lookAhead = UINT32_MAX)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
bool remove(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
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
