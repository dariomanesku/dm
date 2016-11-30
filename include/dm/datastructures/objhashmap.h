/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <stdint.h>
    #include "../check.h"
    #include "../hash.h"
    #include "../compiletime.h"
    #include "../ng/allocatori.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_OBJHASHMAP_H_HEADERGUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_OBJHASHMAP_H_HEADERGUARD
#   define DM_OBJHASHMAP_H_HEADERGUARD
namespace DM_NAMESPACE
{
    template <typename ObjHashMapStorage>
    struct ObjHashMapImpl : ObjHashMapStorage
    {
        /// Expected interface:
        ///
        ///     template <uint8_t KeyLength, typename ObjTy>
        ///     struct ObjHashMapStorageTemplate
        ///     {
        ///         typedef ObjTy ObjectType;
        ///
        ///         struct UsedKey
        ///         {
        ///             uint8_t m_used;
        ///             uint8_t m_key[KeyLen];
        ///         };
        ///
        ///         UsedKey* uk();
        ///         ObjTy* objs();
        ///         uint32_t max();
        ///         uint32_t keyLen();
        ///     };
        typedef typename ObjHashMapStorage::ObjectType ObjTy;
        typedef typename ObjHashMapStorage::UsedKey Uk;
        using ObjHashMapStorage::uk;
        using ObjHashMapStorage::objs;
        using ObjHashMapStorage::max;
        using ObjHashMapStorage::keyLen;

        enum
        {
            Unused   = 0x00,
            Used     = 0x0f,
            FirstHit = 0xff,
            InvalidHandle = UINT32_MAX,
        };

        ObjHashMapImpl() : ObjHashMapStorage()
        {
        }

        void init()
        {
            memset(uk(), Unused, max()*sizeof(Uk));
        }

        ObjTy* insert(const uint8_t* _key, uint8_t _keyLen)
        {
            DM_CHECK(_keyLen <= keyLen(), "ObjHashMapImpl::insert() - Invalid key length | %d, %d", _keyLen, keyLen());

            const uint32_t hash = dm::hash(_key, _keyLen);
            uint32_t idx = wrapAround(hash);
            const uint32_t firstHit = idx;
            for (;;)
            {
                if (Unused == uk()[idx].m_used)
                {
                    uk()[idx].m_used = (idx == firstHit) ? FirstHit : Used;
                    memcpy(&uk()[idx].m_key, _key, _keyLen);
                    return &objs()[idx];
                }

                idx = wrapAround(idx+1);
            }
        }

        ObjTy* insert(const char* _key)
        {
            return insert((const uint8_t*)_key, strlen(_key));
        }

        template <typename Ty>
        ObjTy* insert(const Ty& _key)
        {
            dm_staticAssert(sizeof(Ty) <= ObjHashMapStorage::KeyLen);

            return insert((const uint8_t*)&_key, sizeof(Ty));
        }

        struct ObjDuplicate
        {
            struct
            {
                ObjTy* m_obj;
                bool   m_duplicate;
            };
        };

        ObjDuplicate insertHandleDup(const uint8_t* _key, uint8_t _keyLen)
        {
            DM_CHECK(_keyLen <= keyLen(), "ObjHashMapImpl::insertHandleDup() - Invalid key length | %d, %d", _keyLen, keyLen());

            const uint32_t hash = dm::hash(_key, _keyLen);
            uint32_t idx = wrapAround(hash);
            const uint32_t firstHit = idx;
            for (;;)
            {
                const uint8_t usedFlag = uk()[idx].m_used;

                if (Unused == usedFlag)
                {
                    // Insert new entry.

                    uk()[idx].m_used = (idx == firstHit) ? FirstHit : Used;
                    memcpy(&uk()[idx].m_key, _key, _keyLen);

                    ObjDuplicate result;
                    result.m_obj = &objs()[idx];
                    result.m_duplicate = false;
                    return result;
                }
                else if ((Used & usedFlag)                              // Used
                     &&  0 == memcmp(_key, uk()[idx].m_key, _keyLen))  // && key matches.
                {
                    // Item already found.

                    ObjDuplicate result;
                    result.m_obj = &objs()[idx];
                    result.m_duplicate = true;
                    return result;
                }

                idx = wrapAround(idx+1);
            }
        }

        ObjDuplicate insertHandleDup(const char* _key)
        {
            return insertHandleDup((const uint8_t*)_key, strlen(_key));
        }

        template <typename Ty>
        ObjDuplicate insertHandleDup(const Ty& _key)
        {
            dm_staticAssert(sizeof(Ty) <= ObjHashMapStorage::KeyLen);
            return insertHandleDup((const uint8_t*)&_key, sizeof(Ty));
        }

        ObjTy* find(const uint8_t* _key, uint8_t _keyLen)
        {
            DM_CHECK(_keyLen <= keyLen(), "ObjHashMapImpl::find() - Invalid key length | %d, %d", _keyLen, keyLen());

            const uint32_t hash = dm::hash(_key, _keyLen);
            for (uint32_t idx = wrapAround(hash); true; idx = wrapAround(idx+1))
            {
                const uint8_t usedFlag = uk()[idx].m_used;
                if ((Used & usedFlag)                               // Used
                &&  0 == memcmp(&uk()[idx].m_key, _key, _keyLen))   // && key matches.
                {
                    return &objs()[idx];                            // Return ptr to objs.
                }
                else if (Unused == usedFlag)                        // Unused
                {
                    return NULL;                                    // Return NULL.
                }
            }

            return NULL;
        }

        ObjTy* find(const char* _key)
        {
            return find((const uint8_t*)_key, strlen(_key));
        }

        template <typename Ty>
        ObjTy* find(const Ty& _key)
        {
            dm_staticAssert(sizeof(Ty) <= ObjHashMapStorage::KeyLen);

            return find((const uint8_t*)&_key, sizeof(Ty));
        }

        bool remove(const uint8_t* _key, uint8_t _keyLen)
        {
            DM_CHECK(_keyLen <= keyLen(), "ObjHashMapImpl::remove() - Invalid key length | %d, %d", _keyLen, keyLen());

            ObjTy* obj = find(_key, _keyLen);
            if (NULL != obj)
            {
                const uint32_t handle = obj - objs();
                const uint32_t begin = handle+1;
                uint32_t end = begin;
                while (uk()[end].m_used == Used) { end++; }
                const uint32_t count = end - begin;

                obj->~ObjTy();

                if (0 == count)
                {
                    uk()[handle].m_used = Unused;
                }
                else
                {
                    objs()[handle].~ObjTy();

                    memmove(&uk()[handle],   &uk()[begin],   count*sizeof(Uk));
                    memmove(&objs()[handle], &objs()[begin], count*sizeof(ObjTy));
                    uk()[end].m_used = Unused;
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
            dm_staticAssert(sizeof(Ty) <= ObjHashMapStorage::KeyLen);
            return remove((const uint8_t*)&_key, sizeof(Ty));
        }

    private:
        inline uint32_t wrapAround(uint32_t _v)
        {
            return _v&(max()-1);
        }
    };

    template <uint8_t KeyLength, typename ObjTy, uint32_t MaxT_PowTwo>
    struct ObjHashMapStorageT
    {
        enum
        {
            Max = dm::NextPowTwo<MaxT_PowTwo>::value,
            KeyLen = KeyLength
        };
        typedef ObjTy ObjectType;

        struct UsedKey
        {
            uint8_t m_used;
            uint8_t m_key[KeyLen];
        };

        UsedKey* uk()
        {
            return m_uk;
        }

        ObjTy* objs()
        {
            return m_objs;
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
        UsedKey m_uk[Max];
        ObjTy   m_objs[Max];
    };

    template <uint8_t KeyLength, typename ObjTy>
    struct ObjHashMapStorageExt
    {
        enum { KeyLen = KeyLength };
        typedef ObjTy ObjectType;

        struct UsedKey
        {
            uint8_t m_used;
            uint8_t m_key[KeyLen];
        };

        static uint32_t sizeFor(uint32_t _maxPowTwo)
        {
            DM_CHECK(dm::isPowTwo(_maxPowTwo), "ObjHashMapStorageExt::sizeFor() - Invalid value | %d", _maxPowTwo);

            return _maxPowTwo*(sizeof(UsedKey)+sizeof(ObjTy));
        }

        ObjHashMapStorageExt()
        {
            m_uk = NULL;
            m_max = 0;
        }

        uint8_t* initStorage(uint32_t _maxPowTwo, uint8_t* _mem)
        {
            DM_CHECK(dm::isPowTwo(_maxPowTwo), "ObjHashMapStorageExt::initStorage() - Invalid value | %d", _maxPowTwo);

            m_max = _maxPowTwo;
            m_uk = (UsedKey*)_mem;
            m_objs = (ObjTy*)((uint8_t*)_mem + _maxPowTwo*sizeof(UsedKey));

            return (_mem + sizeFor(_maxPowTwo));
        }

        UsedKey* uk()
        {
            return m_uk;
        }

        ObjTy* objs()
        {
            return m_objs;
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
        UsedKey* m_uk;
        ObjTy*   m_objs;
        uint32_t m_max;
    };

    extern CrtAllocator g_crtAllocator;

    template <uint8_t KeyLength, typename ObjTy>
    struct ObjHashMapStorage
    {
        enum { KeyLen = KeyLength };
        typedef ObjTy ObjectType;

        struct UsedKey
        {
            uint8_t m_used;
            uint8_t m_key[KeyLen];
        };

        static uint32_t sizeFor(uint32_t _maxPowTwo)
        {
            DM_CHECK(dm::isPowTwo(_maxPowTwo), "ObjHashMapStorage::sizeFor() - Invalid value | %d", _maxPowTwo);

            return _maxPowTwo*(sizeof(UsedKey)+sizeof(ObjTy));
        }

        ObjHashMapStorage()
        {
            m_uk = NULL;
            m_max = 0;
        }

        ~ObjHashMapStorage()
        {
            destroy();
        }

        void initStorage(uint32_t _maxPowTwo, AllocatorI* _allocator = &g_crtAllocator)
        {
            DM_CHECK(dm::isPowTwo(_maxPowTwo), "ObjHashMapStorage::initStorage() - Invalid value | %d", _maxPowTwo);

            uint8_t* mem = (uint8_t*)dm_alloc(sizeFor(_maxPowTwo), _allocator);

            m_max = _maxPowTwo;
            m_uk = (UsedKey*)mem;
            m_objs = (ObjTy*)((uint8_t*)mem + _maxPowTwo*sizeof(UsedKey));

            m_allocator = _allocator;
        }

        void destroy()
        {
            if (NULL != m_uk)
            {
                dm_free(m_uk, m_allocator);
                m_uk = NULL;
            }
        }

        UsedKey* uk()
        {
            return m_uk;
        }

        ObjTy* objs()
        {
            return m_objs;
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
        UsedKey* m_uk;
        ObjTy* m_objs;
        uint32_t m_max;
        AllocatorI* m_allocator;
    };

    template <uint8_t KeyLength, typename ObjTy, uint32_t MaxT_PowTwo>
    struct ObjHashMapT : ObjHashMapImpl< ObjHashMapStorageT<KeyLength, ObjTy, MaxT_PowTwo> >
    {
        typedef ObjHashMapImpl< ObjHashMapStorageT<KeyLength, ObjTy, MaxT_PowTwo> > Base;

        ObjHashMapT() : Base()
        {
            Base::init();
        }
    };

    template <uint8_t KeyLength, typename ObjTy>
    struct ObjHashMapExt : ObjHashMapImpl< ObjHashMapStorageExt<KeyLength, ObjTy> >
    {
        typedef ObjHashMapImpl< ObjHashMapStorageExt<KeyLength, ObjTy> > Base;

        uint8_t* init(uint32_t _maxPowTwo, uint8_t* _mem)
        {
            uint8_t* ptr = Base::initStorage(_maxPowTwo, _mem);
            Base::init();
            return ptr;
        }
    };

    template <uint8_t KeyLength, typename ObjTy>
    struct ObjHashMap : ObjHashMapImpl< ObjHashMapStorage<KeyLength, ObjTy> >
    {
        typedef ObjHashMapImpl< ObjHashMapStorage<KeyLength, ObjTy> > Base;

        void init(uint32_t _maxPowTwo, AllocatorI* _allocator = &g_crtAllocator)
        {
            Base::initStorage(_maxPowTwo, _allocator);
            Base::init();
        }
    };

    template <uint8_t KeyLength, typename ObjTy>
    struct ObjHashMapH : ObjHashMapExt<KeyLength, ObjTy>
    {
        AllocatorI* m_allocator;
    };

} // namespace DM_NAMESPACE
#   endif // DM_OBJHASHMAP_H_HEADERGUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
