/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <stdint.h>
    #include "../check.h"
    #include "../compiletime.h" // bestfit_type<>::type, TyInfo<>::Max()
    #include "../allocatori.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_HANDLEALLOC_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_HANDLEALLOC_H_HEADER_GUARD
#   define DM_HANDLEALLOC_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    template <typename HandleAllocStorageTy>
    struct HandleAllocImpl : HandleAllocStorageTy
    {
        /// Expected interface:
        ///     struct HandleAllocStorageTemplate
        ///     {
        ///         typedef typename bestfit_type<MaxHandlesT>::type HandleType;
        ///         HandleType* handles();
        ///         HandleType* indices();
        ///         HandleType  max();
        ///     };
        typedef typename HandleAllocStorageTy::HandleType HandleTy;
        using HandleAllocStorageTy::handles;
        using HandleAllocStorageTy::indices;
        using HandleAllocStorageTy::max;

        HandleAllocImpl() : HandleAllocStorageTy()
        {
        }

        void init()
        {
            m_numHandles = 0;
            for (HandleTy ii = 0, end = max(); ii < end; ++ii)
            {
                handles()[ii] = ii;
            }
        }

        HandleTy alloc()
        {
            DM_CHECK(m_numHandles < max(), "HandleAllocImpl::alloc() | %d, %d", m_numHandles, max());

            const HandleTy index = m_numHandles++;
            const HandleTy handle = handles()[index];
            indices()[handle] = index;

            return handle;
        }

        bool contains(uint32_t _handle)
        {
            DM_CHECK(_handle < max(), "HandleAllocImpl::contains() | %d, %d", _handle, max());

            HandleTy index = indices()[_handle];

            return (index < m_numHandles && handles()[index] == _handle);
        }

        void free(uint32_t _handle)
        {
            DM_CHECK(m_numHandles > 0, "HandleAllocImpl::free() | %d", m_numHandles);

            HandleTy index = indices()[_handle];

            if (index < m_numHandles && handles()[index] == _handle)
            {
                --m_numHandles;
                HandleTy temp = handles()[m_numHandles];
                handles()[m_numHandles] = _handle;
                indices()[temp] = index;
                handles()[index] = temp;
            }
        }

        HandleTy getHandleAt(uint32_t _idx)
        {
            DM_CHECK(_idx < m_numHandles, "HandleAllocImpl::getHandleAt() | %d %d", _idx, m_numHandles);

            return handles()[_idx];
        }

        HandleTy getIdxOf(uint32_t _handle)
        {
            DM_CHECK(_handle < max(), "HandleAllocImpl::getIdxOf() | %d %d", _handle, max());

            return indices()[_handle];
        }

        static int cmpAsc(const void* _a, const void* _b)
        {
            return (*(HandleTy*)_a - *(HandleTy*)_b);
        }

        void sort()
        {
            if (m_numHandles <= 1)
            {
                return;
            }

            qsort(handles(), m_numHandles, sizeof(HandleTy), cmpAsc);

            for (uint32_t ii = 0, end = m_numHandles; ii < end; ++ii)
            {
                HandleTy handle = handles()[ii];
                indices()[handle] = ii;
            }
        }

        void reset()
        {
            m_numHandles = 0;
        }

        HandleTy count()
        {
            return m_numHandles;
        }

    private:
        HandleTy m_numHandles;
    };

    template <uint32_t MaxHandlesT>
    struct HandleAllocStorageT
    {
        typedef typename bestfit_type<MaxHandlesT>::type HandleType;

        HandleType* handles()
        {
            return m_handles;
        }

        HandleType* indices()
        {
            return m_indices;
        }

        HandleType max() const
        {
            return MaxHandlesT;
        }

    private:
        HandleType m_handles[MaxHandlesT];
        HandleType m_indices[MaxHandlesT];
    };

    template <typename HandleTy=uint16_t>
    struct HandleAllocStorageExt
    {
        typedef HandleTy HandleType;

        static uint32_t sizeFor(uint32_t _max)
        {
            return 2*_max*sizeof(HandleType);
        }

        HandleAllocStorageExt()
        {
            m_max = 0;
            m_handles = NULL;
            m_indices = NULL;
        }

        uint8_t* initStorage(uint32_t _max, uint8_t* _mem)
        {
            const uint32_t haSize = _max*sizeof(HandleType);

            m_max = _max;
            m_handles = (HandleType*)_mem;
            m_indices = (HandleType*)((uint8_t*)_mem + haSize);

            return _mem + 2*haSize;
        }

        HandleType* handles()
        {
            return m_handles;
        }

        HandleType* indices()
        {
            return m_indices;
        }

        HandleType max() const
        {
            return m_max;
        }

    private:
        uint32_t m_max;
        HandleType* m_handles;
        HandleType* m_indices;
    };

    extern CrtAllocator g_crtAllocator;

    template <typename HandleTy=uint16_t>
    struct HandleAllocStorage
    {
        typedef HandleTy HandleType;

        static uint32_t sizeFor(uint32_t _max)
        {
            return 2*_max*sizeof(HandleType);
        }

        HandleAllocStorage()
        {
            m_max = 0;
            m_handles = NULL;
            m_indices = NULL;
        }

        ~HandleAllocStorage()
        {
            destroy();
        }

        void initStorage(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            const uint32_t haSize = _max*sizeof(HandleType);
            void* mem = DM_ALLOC(_allocator, 2*haSize);

            m_max = _max;
            m_handles = (HandleType*)mem;
            m_indices = (HandleType*)((uint8_t*)mem + haSize);

            m_allocator = _allocator;
        }

        void destroy()
        {
            if (NULL != m_handles)
            {
                DM_FREE(m_allocator, m_handles);
                m_handles = NULL;
                m_indices = NULL;
            }
        }

        HandleType* handles()
        {
            return m_handles;
        }

        HandleType* indices()
        {
            return m_indices;
        }

        HandleType max() const
        {
            return m_max;
        }

    private:
        uint32_t m_max;
        HandleType* m_handles;
        HandleType* m_indices;
        AllocatorI* m_allocator;
    };

    template <typename HandleTy=uint16_t>
    struct HandleAllocStorageRes
    {
        typedef HandleTy HandleType;

        static uint32_t sizeFor(uint32_t _max)
        {
            return 2*_max*sizeof(HandleType);
        }

        HandleAllocStorageRes()
        {
            m_max = 0;
            m_handles = NULL;
            m_indices = NULL;
        }

        ~HandleAllocStorageRes()
        {
            destroy();
        }

        void initStorage(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            const uint32_t haSize = _max*sizeof(HandleType);

            m_max = _max;
            m_handles = (HandleType*)DM_ALLOC(_allocator, haSize);
            m_indices = (HandleType*)DM_ALLOC(_allocator, haSize);
            m_allocator = _allocator;
        }

        void resize(uint32_t _newMax)
        {
            const uint32_t haSize = _newMax*sizeof(HandleType);
            m_max = _newMax;
            m_handles = (HandleType*)DM_REALLOC(m_allocator, m_handles, haSize);
            m_indices = (HandleType*)DM_REALLOC(m_allocator, m_indices, haSize);
        }

        void expand()
        {
            const uint32_t newMax = m_max+(m_max>>1);
            resize(newMax);
        }

        void destroy()
        {
            if (NULL != m_handles)
            {
                DM_FREE(m_allocator, m_handles);
                m_handles = NULL;
                DM_FREE(m_allocator, m_indices);
                m_indices = NULL;
            }
        }

        HandleType* handles()
        {
            return m_handles;
        }

        HandleType* indices()
        {
            return m_indices;
        }

        HandleType max() const
        {
            return m_max;
        }

    private:
        uint32_t m_max;
        HandleType* m_handles;
        HandleType* m_indices;
        AllocatorI* m_allocator;
    };

    template <uint32_t MaxHandlesT>
    struct HandleAllocT : HandleAllocImpl< HandleAllocStorageT<MaxHandlesT> >
    {
        typedef HandleAllocImpl< HandleAllocStorageT<MaxHandlesT> > Base;

        HandleAllocT()
        {
            Base::init();
        }
    };

    template <typename HandleTy=uint16_t>
    struct HandleAllocExt : HandleAllocImpl< HandleAllocStorageExt<HandleTy> >
    {
        typedef HandleAllocImpl< HandleAllocStorageExt<HandleTy> > Base;

        uint8_t* init(uint32_t _max, uint8_t* _mem)
        {
            uint8_t* ptr = Base::initStorage(_max, _mem);
            Base::init();

            return ptr;
        }
    };

    template <typename HandleTy=uint16_t>
    struct HandleAlloc : HandleAllocImpl< HandleAllocStorage<HandleTy> >
    {
        typedef HandleAllocImpl< HandleAllocStorage<HandleTy> > Base;

        void init(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            Base::initStorage(_max, _allocator);
            Base::init();
        }
    };

    template <typename HandleTy=uint16_t>
    struct HandleAllocRes : HandleAllocImpl< HandleAllocStorageRes<HandleTy> >
    {
        typedef HandleAllocImpl< HandleAllocStorageRes<HandleTy> > Base;

        void init(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            Base::initStorage(_max, _allocator);
            Base::init();
        }
    };

    template <typename HandleTy=uint16_t>
    struct HandleAllocH : HandleAllocExt<HandleTy>
    {
        AllocatorI* m_allocator;
    };

} // namespace DM_NAMESPACE
#   endif // DM_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
