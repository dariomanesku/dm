/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_HANDLEALLOC_HEADER_GUARD
#define DM_NG_HANDLEALLOC_HEADER_GUARD

#include <stdint.h>
#include <dm/check.h>
#include <dm/compiletime.h> // dm::bestfit_type<>::type, TyInfo<>::Max()
#include <dm/ng/allocator.h>

namespace dm { namespace ng {

template <typename HandleAllocStorageTy>
struct HandleAllocImpl : HandleAllocStorageTy
{
    /// Expected interface:
    ///     struct HandleAllocStorageTemplate
    ///     {
    ///         typedef typename dm::bestfit_type<MaxHandlesT>::type HandleType;
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
    typedef typename dm::bestfit_type<MaxHandlesT>::type HandleType;

    HandleType* handles()
    {
        return m_handles;
    }

    HandleType* indices()
    {
        return m_indices;
    }

    HandleType max()
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

    HandleType max()
    {
        return m_max;
    }

private:
    uint32_t m_max;
    HandleType* m_handles;
    HandleType* m_indices;
};

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

    void initStorage(uint32_t _max, ReallocFn _reallocFn = &::realloc)
    {
        const uint32_t haSize = _max*sizeof(HandleType);
        void* mem = dm_alloc(2*haSize, _reallocFn);

        m_max = _max;
        m_handles = (HandleType*)mem;
        m_indices = (HandleType*)((uint8_t*)mem + haSize);

        m_reallocFn = _reallocFn;
    }

    void destroy()
    {
        if (NULL != m_handles)
        {
            dm_free(m_handles, m_reallocFn);
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

    HandleType max()
    {
        return m_max;
    }

private:
    uint32_t m_max;
    HandleType* m_handles;
    HandleType* m_indices;
    ReallocFn m_reallocFn;
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

    void init(uint32_t _max, ReallocFn _reallocFn = &::realloc)
    {
        Base::initStorage(_max, _reallocFn);
        Base::init();
    }
};

template <typename HandleTy=uint16_t>
struct HandleAllocH : HandleAllocExt<HandleTy>
{
    ReallocFn m_reallocFn;
};

} //namespace ng
} //namespace dm

#endif // DM_NG_HANDLEALLOC_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
