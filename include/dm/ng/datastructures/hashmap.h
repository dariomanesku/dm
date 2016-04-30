/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_HASHMAP_HEADER_GUARD
#define DM_NG_HASHMAP_HEADER_GUARD

#include <stdint.h>
#include <dm/misc.h>  // nextPowTwo
#include <dm/check.h>
#include <dm/hash.h>
#include <dm/compiletime.h>
#include <dm/ng/allocator.h>

namespace dm { namespace ng {

template <typename HashMapStorageTy>
struct HashMapImpl : HashMapStorageTy
{
    /// Expected interface:
    ///
    ///     template <uint8_t KeyLength, typename ValTy/*arithmetic type*/>
    ///     struct HashMapStorageTemplate
    ///     {
    ///         typedef ValTy ValueType;
    ///
    ///         struct UsedKeyVal
    ///         {
    ///             uint8_t m_used;
    ///             uint8_t m_key[KeyLen];
    ///             ValTy   m_val;
    ///         };
    ///
    ///         UsedKeyVal* ukv();
    ///         uint32_t max():
    ///         uint32_t keyLen();
    ///     }
    typedef typename HashMapStorageTy::ValueType ValTy;
    typedef typename HashMapStorageTy::UsedKeyVal Ukv;
    using HashMapStorageTy::ukv;
    using HashMapStorageTy::max;
    using HashMapStorageTy::keyLen;

    enum
    {
        Unused   = 0x00,
        Used     = 0x0f,
        FirstHit = 0xff,
        InvalidHandle = UINT32_MAX,
    };

    HashMapImpl() : HashMapStorageTy()
    {
    }

    void init()
    {
        memset(ukv(), Unused, max()*sizeof(Ukv));
    }

    uint32_t insert(const uint8_t* _key, uint8_t _keyLen, ValTy _val)
    {
        DM_CHECK(_keyLen <= keyLen(), "HashMapImpl::insert() - Invalid key length | %d, %d", _keyLen, keyLen());

        const uint32_t hash = dm::hash(_key, _keyLen);
        uint32_t idx = wrapAround(hash);
        const uint32_t firstHit = idx;
        for (;;)
        {
            if (Unused == ukv()[idx].m_used)
            {
                ukv()[idx].m_used = (idx == firstHit) ? FirstHit : Used;
                memcpy(&ukv()[idx].m_key, _key, _keyLen);
                memcpy(&ukv()[idx].m_val, &_val, sizeof(ValTy));
                return idx;
            }

            idx = wrapAround(idx+1);
        }
    }

    uint32_t insert(const char* _key, ValTy _val)
    {
        return insert((const uint8_t*)_key, strlen(_key), _val);
    }

    template <typename Ty>
    uint32_t insert(const Ty& _key, ValTy _val)
    {
        dm_staticAssert(sizeof(Ty) <= HashMapStorageTy::KeyLen);

        return insert((const uint8_t*)&_key, sizeof(Ty), _val);
    }

    struct IdxDuplicate
    {
        union
        {
            uint64_t m_idxDuplicate;

            struct
            {
                uint32_t m_idx;
                uint32_t m_duplicate;
            };
        };

        bool isDuplicate() const
        {
            return (m_duplicate > 0);
        }
    };

    IdxDuplicate insertHandleDup(const uint8_t* _key, uint8_t _keyLen, ValTy _val)
    {
        DM_CHECK(_keyLen <= keyLen(), "HashMapImpl::insertHandleDup() - Invalid key length | %d, %d", _keyLen, keyLen());

        const uint32_t hash = dm::hash(_key, _keyLen);
        uint32_t idx = wrapAround(hash);
        const uint32_t firstHit = idx;
        for (;;)
        {
            const uint8_t usedFlag = ukv()[idx].m_used;

            if (Unused == usedFlag)
            {
                // Insert new entry.

                ukv()[idx].m_used = (idx == firstHit) ? FirstHit : Used;
                memcpy(&ukv()[idx].m_key, _key, _keyLen);
                memcpy(&ukv()[idx].m_val, &_val, sizeof(ValTy));

                IdxDuplicate result;
                result.m_idx = idx;
                result.m_duplicate = 0;
                return result;
            }
            else if ((Used & usedFlag)                              // Used
                 &&  0 == memcmp(_key, ukv()[idx].m_key, _keyLen))  // && key matches.
            {
                // Item already found.

                IdxDuplicate result;
                result.m_idx = idx;
                result.m_duplicate = 1;
                return result;
            }

            idx = wrapAround(idx+1);
        }
    }

    IdxDuplicate insertHandleDup(const char* _key, ValTy _val)
    {
        return insertHandleDup((const uint8_t*)_key, strlen(_key), _val);
    }

    template <typename Ty>
    IdxDuplicate insertHandleDup(const Ty& _key, ValTy _val)
    {
        dm_staticAssert(sizeof(Ty) <= HashMapStorageTy::KeyLen);
        return insertHandleDup((const uint8_t*)&_key, sizeof(Ty), _val);
    }

    uint32_t findHandleOf(const uint8_t* _key, uint8_t _keyLen)
    {
        DM_CHECK(_keyLen <= keyLen(), "HashMapImpl::findHandleOf() - Invalid key length | %d, %d", _keyLen, keyLen());

        const uint32_t hash = dm::hash(_key, _keyLen);
        for (uint32_t idx = wrapAround(hash); true; idx = wrapAround(idx+1))
        {
            const uint8_t usedFlag = ukv()[idx].m_used;
            if ((Used & usedFlag)                               // Used
            &&  0 == memcmp(&ukv()[idx].m_key, _key, _keyLen))  // && key matches.
            {
                return idx;                                     // Return idx;
            }
            else if (Unused == usedFlag)                        // Unused
            {
                return InvalidHandle;                           // Return not found.
            }
        }

        return InvalidHandle;
    }

    uint32_t findHandleOf(const char* _key)
    {
        return findHandleOf(_key, strlen(_key));
    }

    template <typename Ty>
    uint32_t findHandleOf(const Ty& _key)
    {
        dm_staticAssert(sizeof(Ty) <= HashMapStorageTy::KeyLen);

        return findHandleOf((const uint8_t*)&_key, sizeof(Ty));
    }

    ValTy getValueOf(uint32_t _handle)
    {
        return ukv()[_handle].m_val;
    }

    ValTy find(const uint8_t* _key, uint8_t _keyLen)
    {
        DM_CHECK(_keyLen <= keyLen(), "HashMapImpl::find() - Invalid key length | %d, %d", _keyLen, keyLen());

        const uint32_t handle = findHandleOf(_key, _keyLen);
        return (InvalidHandle != handle) ? getValueOf(handle) : dm::TyInfo<ValTy>::Max();
    }

    ValTy find(const char* _key)
    {
        return find((const uint8_t*)_key, strlen(_key));
    }

    template <typename Ty>
    ValTy find(const Ty& _key)
    {
        dm_staticAssert(sizeof(Ty) <= HashMapStorageTy::KeyLen);
        return find((const uint8_t*)&_key, sizeof(Ty));
    }

    bool remove(const uint8_t* _key, uint8_t _keyLen)
    {
        DM_CHECK(_keyLen <= keyLen(), "HashMapImpl::remove() - Invalid key length | %d, %d", _keyLen, keyLen());

        const uint32_t handle = findHandleOf(_key, _keyLen);
        if (InvalidHandle != handle)
        {
            const uint32_t begin = handle+1;
            uint32_t end = begin;
            while (ukv()[end].m_used == Used) { end++; }
            const uint32_t count = end - begin;

            if (0 == count)
            {
                ukv()[handle].m_used = Unused;
            }
            else
            {
                memmove(&ukv()[handle], &ukv()[begin], count*sizeof(Ukv));
                ukv()[end].m_used = Unused;
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    bool remove(const char* _key)
    {
        return remove((const uint8_t*)_key, strlen(_key));
    }

    template <typename Ty>
    bool remove(const Ty& _key)
    {
        dm_staticAssert(sizeof(Ty) <= HashMapStorageTy::KeyLen);
        return remove((const uint8_t*)&_key, sizeof(Ty));
    }

private:
    inline uint32_t wrapAround(uint32_t _v)
    {
        return _v&(max()-1);
    }
};

template <uint8_t KeyLength, typename ValTy/*arithmetic type*/, uint32_t MaxT_PowTwo>
struct HashMapStorageT
{
    enum
    {
        Max = dm::NextPowTwo<MaxT_PowTwo>::value,
        KeyLen = KeyLength
    };
    typedef ValTy ValueType;

    struct UsedKeyVal
    {
        uint8_t m_used;
        uint8_t m_key[KeyLen];
        ValTy   m_val;
    };

    UsedKeyVal* ukv()
    {
        return m_ukv;
    }

    uint32_t max()
    {
        return Max;
    }

    uint32_t keyLen()
    {
        return KeyLen;
    }

private:
    UsedKeyVal m_ukv[Max];
};

template <uint8_t KeyLength, typename ValTy/*arithmetic type*/>
struct HashMapStorageExt
{
    enum { KeyLen = KeyLength };
    typedef ValTy ValueType;

    struct UsedKeyVal
    {
        uint8_t m_used;
        uint8_t m_key[KeyLen];
        ValTy   m_val;
    };

    static uint32_t sizeFor(uint32_t _maxPowTwo)
    {
        DM_CHECK(dm::isPowTwo(_maxPowTwo), "HashMapStorageExt::sizeFor() - Invalid value | %d", _maxPowTwo);

        return _maxPowTwo*sizeof(UsedKeyVal);
    }

    HashMapStorageExt()
    {
        m_ukv = NULL;
        m_max = 0;
    }

    uint8_t* initStorage(uint32_t _maxPowTwo, uint8_t* _mem)
    {
        DM_CHECK(dm::isPowTwo(_maxPowTwo), "HashMapStorageExt::initStorage() - Invalid value | %d", _maxPowTwo);

        m_max = _maxPowTwo;
        m_ukv = (UsedKeyVal*)_mem;

        return (_mem + sizeFor(_maxPowTwo));
    }

    UsedKeyVal* ukv()
    {
        return m_ukv;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint32_t keyLen()
    {
        return KeyLen;
    }

private:
    UsedKeyVal* m_ukv;
    uint32_t m_max;
};

template <uint8_t KeyLength, typename ValTy/*arithmetic type*/>
struct HashMapStorage
{
    enum { KeyLen = KeyLength };
    typedef ValTy ValueType;

    struct UsedKeyVal
    {
        uint8_t m_used;
        uint8_t m_key[KeyLen];
        ValTy   m_val;
    };

    static uint32_t sizeFor(uint32_t _maxPowTwo)
    {
        DM_CHECK(dm::isPowTwo(_maxPowTwo), "HashMapStorage::sizeFor() - Invalid value | %d", _maxPowTwo);

        return _maxPowTwo*sizeof(UsedKeyVal);
    }

    HashMapStorage()
    {
        m_ukv = NULL;
        m_max = 0;
    }

    ~HashMapStorage()
    {
        destroy();
    }

    void initStorage(uint32_t _maxPowTwo, ReallocFn _reallocFn = &::realloc)
    {
        DM_CHECK(dm::isPowTwo(_maxPowTwo), "HashMapStorage::initStorage() - Invalid value | %d", _maxPowTwo);

        uint8_t* mem = (uint8_t*)dm_alloc(sizeFor(_maxPowTwo), _reallocFn);

        m_max = _maxPowTwo;
        m_ukv = (UsedKeyVal*)mem;
        m_reallocFn = _reallocFn;
    }

    void destroy()
    {
        if (NULL != m_ukv)
        {
            dm_free(m_ukv, m_reallocFn);
            m_ukv = NULL;
        }
    }

    UsedKeyVal* ukv()
    {
        return m_ukv;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint32_t keyLen()
    {
        return KeyLen;
    }

private:
    UsedKeyVal* m_ukv;
    uint32_t m_max;
    ReallocFn m_reallocFn;
};

template <uint8_t KeyLength, typename ValTy, uint32_t MaxT_PowTwo>
struct HashMapT : HashMapImpl< HashMapStorageT<KeyLength, ValTy, MaxT_PowTwo> >
{
    typedef HashMapImpl< HashMapStorageT<KeyLength, ValTy, MaxT_PowTwo> > Base;

    HashMapT() : Base()
    {
        Base::init();
    }
};

template <uint8_t KeyLength, typename ValTy>
struct HashMapExt : HashMapImpl< HashMapStorageExt<KeyLength, ValTy> >
{
    typedef HashMapImpl< HashMapStorageExt<KeyLength, ValTy> > Base;

    uint8_t* init(uint32_t _maxPowTwo, uint8_t* _mem)
    {
        uint8_t* ptr = Base::initStorage(_maxPowTwo, _mem);
        Base::init();
        return ptr;
    }
};

template <uint8_t KeyLength, typename ValTy>
struct HashMap : HashMapImpl< HashMapStorage<KeyLength, ValTy> >
{
    typedef HashMapImpl< HashMapStorage<KeyLength, ValTy> > Base;

    void init(uint32_t _maxPowTwo, ReallocFn _reallocFn = &::realloc)
    {
        Base::initStorage(_maxPowTwo, _reallocFn);
        Base::init();
    }
};

template <uint8_t KeyLength, typename ValTy>
struct HashMapH : HashMapExt<KeyLength, ValTy>
{
    ReallocFn m_reallocFn;
};

} //namespace ng
} //namespace dm

#endif // DM_NG_HASHMAP_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
