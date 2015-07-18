/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

//TODO: Not safe. infinite loop if hashmap is full !
template <typename PtrTy>
uint32_t insert(const PtrTy* _key, ValTy _val)
{
    const uint32_t hash = dm::hash(_key, KeyLen);
    uint32_t idx = wrapAround(hash);
    for (;;)
    {
        if (Unused == m_ukv[idx].m_used)
        {
            m_ukv[idx].m_used = Used;
            memcpy(m_ukv[idx].m_key, _key, KeyLen);
            memcpy(&m_ukv[idx].m_val, &_val, sizeof(_val));
            return idx;
        }

        idx = wrapAround(idx+1);
    }
}

template <typename Ty>
uint32_t insert(Ty _key, ValTy _val)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return insert((const void*)&_key, _val);
}

struct CollisionIdx
{
    union
    {
        uint64_t m_collisionIdx;

        struct
        {
            uint32_t m_collision;
            uint32_t m_idx;
        };
    };

    bool isCollision() const
    {
        return (m_collision > 0);
    }
};

template <typename PtrTy>
CollisionIdx insertHandleCollision(const PtrTy* _key, ValTy _val)
{
    CollisionIdx result;

    const uint32_t hash = dm::hash(_key, KeyLen);
    uint32_t idx = wrapAround(hash);
    for (;;)
    {
        if (Used == m_ukv[idx].m_used                       // Used
        &&     0 == memcmp(_key, m_ukv[idx].m_key, KeyLen)) // && key matches.
        {
            // Collision found.

            result.m_collision = 1;
            result.m_idx = idx;
            return result;
        }

        if (Unused == m_ukv[idx].m_used)
        {
            // Insert new entry.

            m_ukv[idx].m_used = Used;
            memcpy(m_ukv[idx].m_key, _key, KeyLen);
            memcpy(&m_ukv[idx].m_val, &_val, sizeof(_val));

            result.m_collision = 0;
            result.m_idx = idx;
            return result;
        }

        idx = wrapAround(idx+1);
    }
}

template <typename Ty>
CollisionIdx insertHandleCollision(Ty _key, ValTy _val)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return insertHandleCollision((const void*)&_key, _val);
}

template <typename PtrTy>
uint32_t unsafeFindHandleOf(const PtrTy* _key)
{
    const uint32_t hash = dm::hash(_key);
    uint32_t idx = wrapAround(hash);
    for (;;)
    {
        if (Used == m_ukv[idx].m_used                       // Used
        &&     0 == memcmp(_key, m_ukv[idx].m_key, KeyLen)) // && key matches.
        {
            return idx;                                     // Return idx;
        }

        idx = wrapAround(idx+1);                            // Iterate to the next slot.
    }
}

template <typename Ty>
uint32_t unsafeFindHandleOf(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return unsafeFindHandleOf((const void*)&_key);
}

ValTy getValueOf(uint32_t _handle)
{
    return m_ukv[_handle].m_val;
}

template <typename PtrTy>
ValTy unsafeFind(const PtrTy* _key)
{
    return getValueOf(unsafeFindHandleOf(_key));
}

template <typename Ty>
ValTy unsafeFind(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return unsafeFind((const void*)&_key);
}

template <typename PtrTy>
uint32_t findIdxOf(const PtrTy* _key, uint32_t _lookAhead = UINT32_MAX)
{
    const uint32_t hash = dm::hash(_key);
    uint32_t idx  = wrapAround(hash);
    for (uint32_t ii = 0, end = (UINT32_MAX == _lookAhead) ? max() : _lookAhead; ii < end; ++ii)
    {
        if (Used == m_ukv[idx].m_used                       // Used
        &&     0 == memcmp(_key, m_ukv[idx].m_key, KeyLen)) // && key matches.
        {
            return idx;                                     // Return idx.
        }

        idx = wrapAround(idx+1);                            // Iterate to the next slot.
    }

    return InvalidIdx;
}

template <typename Ty>
uint32_t findIdxOf(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return findIdxOf((const void*)&_key, _lookAhead);
}

template <typename PtrTy>
ValTy find(const PtrTy* _key, uint32_t _lookAhead = UINT32_MAX)
{
    const uint32_t idx = findIdxOf(_key, _lookAhead);
    if (InvalidIdx != idx)
    {
        return m_ukv[idx].m_val;
    }
    else
    {
        return dm::TyInfo<ValTy>::Max();
    }
}

template <typename Ty>
ValTy find(Ty _key, uint32_t _lookAhead = UINT32_MAX)
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
ValTy unsafeRemove(const PtrTy* _key)
{
    const uint32_t idx = unsafeFindHandleOf(_key);
    m_ukv[idx].m_used = Unused;
    return m_ukv[idx].m_val;
}

template <typename Ty>
ValTy unsafeRemove(Ty _key)
{
    dm_staticAssert(is_arithmetic<Ty>::value);
    dm_staticAssert(sizeof(Ty) <= KeyLen);

    return unsafeRemove((const void*)&_key);
}

template <typename PtrTy>
bool remove(const PtrTy* _key, uint32_t _lookAhead = UINT32_MAX)
{
    const uint32_t idx = findIdxOf(_key, _lookAhead);
    if (InvalidIdx != idx)
    {
        m_ukv[idx].m_used = Unused;
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

private:
inline uint32_t wrapAround(uint32_t _v)
{
    return _v&(max()-1);
}
public:

/* vim: set sw=4 ts=4 expandtab: */
