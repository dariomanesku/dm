/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

// Notice: It gives cryptic compile errors but it does the job.
#define DM_ASSERT_KEYLEN_FITS_IN_TYPE(_ty) typename dm::enable_if<sizeof(_ty) <= KeyLen, _ty>::type* = nullptr

//TODO: Not safe. infinite loop if hashmap is full !
uint32_t insert(const void* _key, ValTy _val)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
uint32_t insert(Ty _key, ValTy _val)
{
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

CollisionIdx insertHandleCollision(const void* _key, ValTy _val)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
CollisionIdx insertHandleCollision(Ty _key, ValTy _val)
{
    return insertHandleCollision((const void*)&_key, _val);
}

uint32_t unsafeFindHandleOf(const void* _key)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
uint32_t unsafeFindHandleOf(Ty _key)
{
    return unsafeFindHandleOf((const void*)&_key);
}

ValTy getValueOf(uint32_t _handle)
{
    return m_ukv[_handle].m_val;
}

ValTy unsafeFind(const void* _key)
{
    return getValueOf(unsafeFindHandleOf(_key));
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
ValTy unsafeFind(Ty _key)
{
    return unsafeFind((const void*)&_key);
}

uint32_t findIdxOf(const void* _key, uint32_t _lookAhead = UINT32_MAX)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
uint32_t findIdxOf(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    return findIdxOf((const void*)&_key, _lookAhead);
}

ValTy find(const void* _key, uint32_t _lookAhead = UINT32_MAX)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
ValTy find(Ty _key, uint32_t _lookAhead = UINT32_MAX)
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

ValTy unsafeRemove(const void* _key)
{
    const uint32_t idx = unsafeFindHandleOf(_key);
    m_ukv[idx].m_used = Unused;
    return m_ukv[idx].m_val;
}

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
ValTy unsafeRemove(Ty _key)
{
    return unsafeRemove((const void*)&_key);
}

bool remove(const void* _key, uint32_t _lookAhead = UINT32_MAX)
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

template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic), DM_ASSERT_KEYLEN_FITS_IN_TYPE(Ty)>
bool remove(Ty _key, uint32_t _lookAhead = UINT32_MAX)
{
    return remove((const void*)&_key, _lookAhead);
}

private:
inline uint32_t wrapAround(uint32_t _v)
{
    return _v&(max()-1);
}
public:

/* vim: set sw=4 ts=4 expandtab: */
