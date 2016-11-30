/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <new>
    #include "../check.h"
    #include "../allocatori.h"
    #include "handlealloc.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_SPARSEARRAY_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_SPARSEARRAY_HEADER_GUARD
#   define DM_SPARSEARRAY_HEADER_GUARD
namespace DM_NAMESPACE
{
    template <typename SparseArrayStorageTy>
    struct SparseArrayImpl : SparseArrayStorageTy
    {
        /// Expected interface:
        ///
        ///     template <typename Ty>
        ///     struct SparseArrayStorageT
        ///     {
        ///         typedef Ty ObjectType;
        ///         typedef HandleAllocT<MaxT> HandleAllocType;
        ///
        ///         bool isResizable() const;
        ///         Ty* elements();
        ///         HandleAllocType* handles();
        ///         uint32_t max() const;
        ///     };
        typedef typename SparseArrayStorageTy::ObjectType Ty;
        typedef typename SparseArrayStorageTy::HandleAllocType HandleAllocTy;
        using SparseArrayStorageTy::isResizable;
        using SparseArrayStorageTy::resize;
        using SparseArrayStorageTy::elements;
        using SparseArrayStorageTy::handles;
        using SparseArrayStorageTy::max;

        SparseArrayImpl() : SparseArrayStorageTy()
        {
        }

        Ty* addNew()
        {
            if (isResizable())
            {
                const uint32_t maxObj = max();

                if (count() == maxObj)
                {
                    const uint32_t newMax = maxObj+(maxObj>>1);
                    resize(newMax);
                }
            }

            const uint32_t handle = (uint32_t)handles()->alloc();
            DM_CHECK(handle < max(), "SparseArrayImpl::addNew() | %d, %d", handle, max());

            Ty* dst = &elements()[handle];
            dst = ::new (dst) Ty();
            return dst;
        }

        uint32_t addObj(const Ty* _obj)
        {
            if (isResizable())
            {
                const uint32_t maxObj = max();

                if (count() == maxObj)
                {
                    const uint32_t newMax = maxObj+(maxObj>>1);
                    resize(newMax);
                }
            }

            const uint32_t handle = (uint32_t)handles()->alloc();
            DM_CHECK(handle < max(), "SparseArrayImpl::addCopy() | %d, %d", handle, max());

            Ty* dst = &elements()[handle];
            dst = ::new (dst) Ty(*_obj);
            return handle;
        }

        uint32_t addVal(Ty _val)
        {
            if (isResizable())
            {
                const uint32_t maxObj = max();

                if (count() == maxObj)
                {
                    const uint32_t newMax = maxObj+(maxObj>>1);
                    resize(newMax);
                }
            }

            const uint32_t handle = (uint32_t)handles()->alloc();
            DM_CHECK(handle < max(), "SparseArrayImpl::addCopy() | %d, %d", handle, max());

            elements()[handle] = _val;
            return handle;
        }

        bool contains(const Ty* _obj)
        {
            return (&elements()[0] <= _obj && _obj < &elements()[max()]);
        }

        uint32_t getHandleOf(const Ty* _obj)
        {
            DM_CHECK(contains(_obj), "SparseArrayImpl::getHandleOf() | Object not from the list.");

            return uint32_t(_obj - elements());
        }

        Ty& operator[](uint32_t _handle)
        {
            DM_CHECK(_handle < max(), "SparseArrayImpl::operator[]() | %d, %d", _handle, max());

            return elements()[_handle];
        }

        const Ty& operator[](uint32_t _handle) const
        {
            DM_CHECK(_handle < max(), "SparseArrayImpl::operator[]() const | %d, %d", _handle, max());

            return elements()[_handle];
        }

        Ty* getObj(uint32_t _handle)
        {
            DM_CHECK(_handle < max(), "SparseArrayImpl::getObj() | %d, %d", _handle, max());

            return &elements()[_handle];
        }

        Ty getVal(uint32_t _handle)
        {
            DM_CHECK(_handle < max(), "SparseArrayImpl::getVal() | %d, %d", _handle, max());

            return elements()[_handle];
        }

        /// Used for iteration over all elements.
        uint32_t getHandleAt(uint32_t _idx)
        {
            return handles()->getHandleAt(_idx);
        }

        /// Used for iteration over all elements.
        Ty* getObjFromHandleAt(uint32_t _idx)
        {
            const uint32_t handle = handles()->getHandleAt(_idx);
            return getObj(handle);
        }

        /// Used for iteration over all elements.
        Ty* getValFromHandleAt(uint32_t _idx)
        {
            const uint32_t handle = handles()->getHandleAt(_idx);
            return getVal(handle);
        }

        void remove(uint32_t _handle)
        {
            DM_CHECK(_handle < max(), "SparseArrayImpl::removeAt() | %d, %d", _handle, max());

            elements()[_handle].~Ty();
            handles()->free(_handle);
        }

        void removeFromHandleAt(uint32_t _idx)
        {
            const uint32_t handle = handles()->getHandleAt(_idx);
            remove(handle);
        }

        void removeObj(const Ty* _obj)
        {
            const uint32_t handle = getHandleOf(_obj);
            remove(handle);
        }

        void removeAll()
        {
            for (uint32_t ii = count(); ii--; )
            {
                const uint32_t handle = handles()->getHandleAt(ii);
                remove(handle);
            }
        }

        void compact()
        {
            const uint32_t end = count();
            if (end <= 1)
            {
                return;
            }

            handles()->sort();

            uint32_t idx = 0;
            uint32_t prev;
            uint32_t curr = handles()->getHandleAt(0);

            bool inOrder = (0 == curr);
            for (;;)
            {
                prev = curr;

                if (inOrder)
                {
                    idx++;
                }
                else
                {
                    uint32_t dst = 0;
                    uint32_t src = curr;

                    uint32_t cnt = 1;
                    for (; cnt < end; ++cnt)
                    {
                        const uint32_t next = handles()->getHandleAt(cnt);
                        if (next-prev != 1)
                        {
                            break;
                        }
                        prev = next;
                    }

                    if (cnt > curr)
                    {
                        memmove(&elements()[dst], &elements()[src], cnt*sizeof(Ty));
                    }
                    else
                    {
                        memcpy(&elements()[dst], &elements()[src], cnt*sizeof(Ty));
                    }

                    idx += cnt;
                }

                if (idx >= end)
                {
                    break;
                }

                curr = handles()->getHandleAt(idx);
                const uint32_t diff = curr - prev;
                inOrder = (1 == diff);
            }

            // Reset handle alloc.
            typename HandleAllocTy::HandleTy* han = handles()->handles();
            typename HandleAllocTy::HandleTy* ind = handles()->indices();
            for (uint32_t ii = 0, iiEnd = handles()->max(); ii < iiEnd; ++ii) { han[ii] = ii; }
            for (uint32_t ii = 0; ii < end; ++ii)                             { ind[ii] = ii; }
        }

        void reset()
        {
            handles().reset();
        }

        void zero()
        {
            memset(elements(), 0, max()*sizeof(Ty));
        }

        uint32_t count()
        {
            return handles()->count();
        }
    };

    template <typename Ty, uint32_t MaxT>
    struct SparseArrayStorageT
    {
        typedef Ty ObjectType;
        typedef HandleAllocT<MaxT> HandleAllocType;

        bool isResizable() const
        {
            return false;
        }

        void resize(uint32_t /*_max*/)
        {
        }

        Ty* elements()
        {
            return m_elements;
        }

        HandleAllocType* handles()
        {
            return &m_handles;
        }

        uint32_t max() const
        {
            return MaxT;
        }

        HandleAllocType m_handles;
        Ty m_elements[MaxT];
    };

    template <typename Ty>
    struct SparseArrayStorageExt
    {
        typedef Ty ObjectType;
        typedef HandleAllocExt<uint32_t> HandleAllocType;

        bool isResizable() const
        {
            return false;
        }

        void resize(uint32_t /*_max*/)
        {
        }

        static uint32_t sizeFor(uint32_t _max)
        {
            return _max*sizeof(Ty) + HandleAllocType::sizeFor(_max);
        }

        SparseArrayStorageExt()
        {
            m_max = 0;
            m_elements = NULL;
        }

        uint8_t* init(uint32_t _max, uint8_t* _mem)
        {
            uint8_t* objBegin   = (uint8_t*)_mem;
            uint8_t* handleBegin = (uint8_t*)_mem + _max*sizeof(Ty);

            m_max = _max;
            m_elements = (Ty*)objBegin;
            uint8_t* end = m_handles.init(_max, handleBegin);

            return end;
        }

        Ty* elements()
        {
            return m_elements;
        }

        HandleAllocType* handles()
        {
            return &m_handles;
        }

        uint32_t max() const
        {
            return m_max;
        }

        uint32_t m_max;
        Ty* m_elements;
        HandleAllocType m_handles;
    };

    extern CrtAllocator g_crtAllocator;

    template <typename Ty>
    struct SparseArrayStorage
    {
        typedef Ty ObjectType;
        typedef HandleAllocExt<uint32_t> HandleAllocType;

        bool isResizable() const
        {
            return false;
        }

        void resize(uint32_t /*_max*/)
        {
        }

        static uint32_t sizeFor(uint32_t _max)
        {
            return _max*sizeof(Ty) + HandleAllocType::sizeFor(_max);
        }

        SparseArrayStorage()
        {
            m_max = 0;
            m_elements = NULL;
        }

        void init(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            const uint32_t totalSize = sizeFor(_max);
            void* mem = DM_ALLOC(_allocator, totalSize);

            uint8_t* objBegin    = (uint8_t*)mem;
            uint8_t* handleBegin = (uint8_t*)mem + _max*sizeof(Ty);

            m_max = _max;
            m_elements = (Ty*)objBegin;
            m_handles.init(_max, handleBegin);

            m_allocator = _allocator;
        }

        void destroy()
        {
            if (NULL != m_elements)
            {
                DM_FREE(m_allocator, m_elements);
                m_elements = NULL;
            }
        }

        Ty* elements()
        {
            return m_elements;
        }

        HandleAllocType* handles()
        {
            return &m_handles;
        }

        uint32_t max() const
        {
            return m_max;
        }

        uint32_t m_max;
        Ty* m_elements;
        HandleAllocType m_handles;
        AllocatorI* m_allocator;
    };

    template <typename Ty>
    struct SparseArrayStorageRes
    {
        typedef Ty ObjectType;
        typedef HandleAllocRes<uint32_t> HandleAllocType;

        SparseArrayStorageRes()
        {
            m_max = 0;
            m_elements = NULL;
        }

        void init(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            const uint32_t size = _max*sizeof(Ty);

            m_max = _max;
            m_elements = (Ty*)DM_ALLOC(_allocator, size);
            m_handles.init(_max, _allocator);

            m_allocator = _allocator;
        }

        bool isResizable() const
        {
            return true;
        }

        void resize(uint32_t _newMax)
        {
            const uint32_t size = _newMax*sizeof(Ty);
            m_max = _newMax;
            m_elements = (Ty*)dm_realloc(size, m_allocator);
            m_handles.resize(_newMax);
        }

        void destroy()
        {
            if (NULL != m_elements)
            {
                DM_FREE(m_allocator, m_elements);
                m_elements = NULL;
            }
        }

        Ty* elements()
        {
            return m_elements;
        }

        HandleAllocType* handles()
        {
            return &m_handles;
        }

        uint32_t max() const
        {
            return m_max;
        }

        uint32_t m_max;
        Ty* m_elements;
        HandleAllocType m_handles;
        AllocatorI* m_allocator;
    };

    template <typename Ty, uint32_t MaxT> struct SparseArrayT   : SparseArrayImpl< SparseArrayStorageT<Ty, MaxT> > { };
    template <typename Ty>                struct SparseArrayExt : SparseArrayImpl< SparseArrayStorageExt<Ty>     > { };
    template <typename Ty>                struct SparseArray    : SparseArrayImpl< SparseArrayStorage<Ty>        > { };
    template <typename Ty>                struct SparseArrayRes : SparseArrayImpl< SparseArrayStorage<Ty>        > { };
    template <typename Ty>                struct SparseArrayH   : SparseArrayExt<Ty> { AllocatorI* m_allocator; };

} // namespace DM_NAMESPACE
#   endif // DM_SPARSEARRAY_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
