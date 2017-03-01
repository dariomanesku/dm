/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
#   include <new>
#   include <string.h> // memmove()
#   include "../check.h"
#   include "../allocatori.h"
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_ARRAY_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_ARRAY_H_HEADER_GUARD
#   define DM_ARRAY_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    template <typename ArrayStorageTy>
    struct ArrayImpl : ArrayStorageTy
    {
        /// Expected interface:
        ///
        ///     template <typename Ty>
        ///     struct ArrayStorageTemplate
        ///     {
        ///         typedef Ty ElementType;
        ///         bool isResizable();
        ///         bool resize(uint32_t _max);
        ///         Ty* elements();
        ///         const Ty* elements() const;
        ///         uint32_t max() const;
        ///     };
        typedef typename ArrayStorageTy::ElementType ElemTy;
        using ArrayStorageTy::isResizable;
        using ArrayStorageTy::resize;
        using ArrayStorageTy::elements;
        using ArrayStorageTy::max;

        ArrayImpl() : ArrayStorageTy()
        {
            m_count = 0;
        }

        ElemTy* addNew(uint32_t _count = 1)
        {
            if (isResizable())
            {
                expandIfNecessaryToMakeRoomFor(_count);
            }

            DM_CHECK((m_count+_count) <= max(), "ArrayImpl::addNew() | %d, %d, %d", m_count, _count, max());

            const uint32_t curr = m_count;
            m_count += _count;
            return &elements()[curr];
        }

        void push(ElemTy _value)
        {
            ElemTy* elem = addNew(1);
            *elem = _value;
        }

        ElemTy pop()
        {
            DM_CHECK(0 < m_count, "ArrayImpl::pop() | %d", m_count);

            return elements()[--m_count];
        }

        void pop(uint32_t _count)
        {
            DM_CHECK(_count <= m_count, "ArrayImpl::pop(_count) | %d %d", _count, m_count);

            m_count -= _count;
        }

        ElemTy remove(uint32_t _idx)
        {
            DM_CHECK(0 < m_count && m_count <= max(), "ArrayImpl::remove() - 0 | %d, %d", m_count, max());
            DM_CHECK(_idx < max(), "ArrayImpl::remove() - 1 | %d, %d", _idx, max());

            const ElemTy val = elements()[_idx];

            ElemTy* elem = &elements()[_idx];
            ElemTy* next = &elements()[_idx+1];
            memmove(elem, next, (m_count-_idx-1)*sizeof(ElemTy));
            --m_count;

            return val;
        }

        // Uses swap instead of memmove. Order is not preserved!
        ElemTy removeSwap(uint32_t _idx)
        {
            DM_CHECK(0 < m_count && m_count <= max(), "ArrayImpl::removeSwap() - 0 | %d, %d", m_count, max());
            DM_CHECK(_idx < max(), "ArrayImpl::removeSwap() - 1 | %d, %d", _idx, max());

            const ElemTy val = elements()[_idx];
            elements()[_idx] = elements()[--m_count];
            return val;
        }

        void cut(uint32_t _newCount)
        {
            DM_CHECK(_newCount <= m_count, "ArrayImpl::cut() - 0 | %d, %d", _newCount, m_count);

            m_count = _newCount;
        }

        ElemTy get(uint32_t _idx)
        {
            DM_CHECK(_idx < max(), "ArrayImpl::get() | %d, %d", _idx, max());

            return elements()[_idx];
        }

        ElemTy& operator[](uint32_t _idx)
        {
            DM_CHECK(_idx < max(), "ArrayImpl::operator[]() ref | %d, %d", _idx, max());

            return elements()[_idx];
        }

        const ElemTy& operator[](uint32_t _idx) const
        {
            DM_CHECK(_idx < max(), "ArrayImpl::operator[]() const ref | %d, %d", _idx, max());

            return elements()[_idx];
        }

        void zero()
        {
            memset(elements(), 0, max()*sizeof(ElemTy));
        }

        void fillWith(ElemTy _value)
        {
            for (uint32_t ii = max(); ii--; )
            {
                elements()[ii] = _value;
            }
        }

        void reset()
        {
            m_count = 0;
        }

        ElemTy last()
        {
            return elements()[m_count-1];
        }

        uint32_t count() const
        {
            return m_count;
        }

    private:
        void expandIfNecessaryToMakeRoomFor(uint32_t _count)
        {
            const uint32_t needed = m_count + _count;
            const uint32_t maxElem = max();
            if (needed > maxElem)
            {
                const uint32_t proposedMax = maxElem + (maxElem>>1);
                const uint32_t newMax = (proposedMax > needed) ? proposedMax : needed;
                resize(newMax);
                m_count = newMax < m_count ? newMax : m_count;
            }
        }

        uint32_t m_count;
    };

    template <typename ArrayStorageTy>
    struct ObjArrayImpl : ArrayStorageTy
    {
        typedef typename ArrayStorageTy::ElementType ElemTy;
        using ArrayStorageTy::isResizable;
        using ArrayStorageTy::resize;
        using ArrayStorageTy::elements;
        using ArrayStorageTy::max;

        ObjArrayImpl() : ArrayStorageTy()
        {
            m_count = 0;
        }

        ElemTy* addNew(uint32_t _count = 1)
        {
            if (isResizable())
            {
                expandIfNecessaryToMakeRoomFor(_count);
            }

            DM_CHECK((m_count+_count) <= max(), "ObjArrayImpl::addNew(_count) | %d, %d, %d", m_count, _count, max());

            const uint32_t beg = m_count;
            const uint32_t end = beg + _count;
            for (uint32_t ii = beg; ii < beg; ++ii)
            {
                ElemTy* elem = &elements()[ii];
                elem = ::new (elem) ElemTy();
            }

            m_count = end;
            return &elements()[beg];
        }

        void addCopy(const ElemTy* _obj)
        {
            ElemTy* elem = addNew(1);
            ::new (elem) ElemTy(*_obj);
        }

        void pop(uint32_t _count = 1)
        {
            DM_CHECK(_count <= m_count, "ObjArrayImpl::pop(_count) | %d", m_count);

            const uint32_t newCount = m_count-_count;
            for (uint32_t ii = newCount, end = m_count; ii < end; ++ii)
            {
                elements()[ii].~ElemTy();
            }
            m_count = newCount;
        }

        bool contains(const ElemTy* _obj)
        {
            return (&elements()[0] <= _obj && _obj < &elements()[max()]);
        }

        uint32_t indexOf(const ElemTy* _obj)
        {
            DM_CHECK(this->contains(_obj), "ObjArrayImpl::indexOf() - Invalid input");

            return _obj - &elements()[0];
        }

        void remove(uint32_t _idx)
        {
            DM_CHECK(0 < m_count && m_count <= max(), "ObjArrayImpl::remove() - 0 | %d, %d", m_count, max());
            DM_CHECK(_idx < max(), "ObjArrayImpl::remove() - 1 | %d, %d", _idx, max());

            ElemTy* elem = &elements()[_idx];
            ElemTy* next = &elements()[_idx+1];

            elem->~ElemTy();
            memmove(elem, next, (m_count-_idx-1)*sizeof(ElemTy));

            --m_count;
        }

        // Uses swap instead of memmove. Order is not preserved!
        void removeSwap(uint32_t _idx)
        {
            DM_CHECK(0 < m_count && m_count <= max(), "ObjArrayImpl::removeSwap() - 0 | %d, %d", m_count, max());
            DM_CHECK(_idx < max(), "ObjArrayImpl::removeSwap() - 1 | %d, %d", _idx, max());

            ElemTy* elem = &elements()[_idx];
            elem->~ElemTy();

            if (_idx != --m_count)
            {
                const ElemTy* last = &elements()[m_count];
                elem = ::new (elem) ElemTy(*last);
            }
        }

        void removeSwap(ElemTy* _obj)
        {
            removeSwap(this->indexOf(_obj));
        }

        void removeAll()
        {
            for (uint32_t ii = m_count; ii--; )
            {
                elements()[ii].~ElemTy();
            }
            m_count = 0;
        }

        ElemTy* get(uint32_t _idx)
        {
            DM_CHECK(_idx < max(), "ObjArrayImpl::get() | %d, %d", _idx, max());

            return &elements()[_idx];
        }

        ElemTy& operator[](uint32_t _idx)
        {
            DM_CHECK(_idx < max(), "ObjArrayImpl::operator[]() | %d, %d", _idx, max());

            return elements()[_idx];
        }

        const ElemTy& operator[](uint32_t _idx) const
        {
            DM_CHECK(_idx < max(), "ObjArrayImpl::operator[]() const | %d, %d", _idx, max());

            return elements()[_idx];
        }

        void zero()
        {
            memset(elements(), 0, max()*sizeof(ElemTy));
        }

        void reset()
        {
            m_count = 0;
        }

        ElemTy* last()
        {
            return &elements()[m_count-1];
        }

        uint32_t count() const
        {
            return m_count;
        }

    private:
        void expandIfNecessaryToMakeRoomFor(uint32_t _count)
        {
            const uint32_t needed = m_count + _count;
            const uint32_t maxElem = max();
            if (needed > maxElem)
            {
                const uint32_t proposedMax = maxElem + (maxElem>>1);
                const uint32_t newMax = (proposedMax > needed) ? proposedMax : needed;
                resize(newMax);
                m_count = newMax < m_count ? newMax : m_count;
            }
        }

        uint32_t m_count;
    };

    template <typename Ty, uint32_t MaxT>
    struct ArrayStorageT
    {
        typedef Ty ElementType;

        bool isResizable()
        {
            return false;
        }

        bool resize(uint32_t /*_max*/)
        {
            return false;
        }

        Ty* elements()
        {
            return m_elements;
        }

        const Ty* elements() const
        {
            return m_elements;
        }

        uint32_t max() const
        {
            return MaxT;
        }

        Ty m_elements[MaxT];
    };

    template <typename Ty>
    struct ArrayStorageExt
    {
        typedef Ty ElementType;

        static uint32_t sizeFor(uint32_t _max)
        {
            return _max*sizeof(Ty);
        }

        ArrayStorageExt()
        {
            m_elements = NULL;
            m_max = 0;
        }

        uint8_t* init(uint32_t _max, uint8_t* _mem)
        {
            m_max = _max;
            m_elements = (Ty*)_mem;

            return (_mem + sizeFor(_max));
        }

        bool isResizable()
        {
            return false;
        }

        bool resize(uint32_t /*_max*/)
        {
            return false;
        }

        Ty* elements()
        {
            return m_elements;
        }

        const Ty* elements() const
        {
            return m_elements;
        }

        uint32_t max() const
        {
            return m_max;
        }

        Ty* m_elements;
        uint32_t m_max;
    };

    extern CrtAllocator g_crtAllocator;

    template <typename Ty>
    struct ArrayStorage
    {
        typedef Ty ElementType;

        ArrayStorage()
        {
            m_elements = NULL;
            m_max = 0;
        }

        ArrayStorage(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            init(_max, _allocator);
        }

        ~ArrayStorage()
        {
            destroy();
        }

        void init(uint32_t _max, AllocatorI* _allocator = &g_crtAllocator)
        {
            m_elements = (Ty*)DM_ALLOC(_allocator, _max*sizeof(Ty));
            m_max = _max;
            m_allocator = _allocator;
        }

        void destroy()
        {
            if (NULL != m_elements
            &&  NULL != m_allocator)
            {
                DM_FREE(m_allocator, m_elements);
                m_elements = NULL;
            }
        }

        bool isInitialized() const
        {
            return (NULL != m_elements);
        }

        bool isResizable() const
        {
            return true;
        }

        bool resize(uint32_t _max)
        {
            m_elements = (Ty*)DM_REALLOC(m_allocator, m_elements, _max*sizeof(Ty));
            m_max = _max;

            return true;
        }

        Ty* elements()
        {
            return m_elements;
        }

        const Ty* elements() const
        {
            return m_elements;
        }

        uint32_t max() const
        {
            return m_max;
        }

        Ty* m_elements;
        uint32_t m_max;
        AllocatorI* m_allocator;
    };

    template <typename Ty, uint32_t MaxTy> struct ArrayT   : ArrayImpl< ArrayStorageT<Ty, MaxTy> > { };
    template <typename Ty>                 struct ArrayExt : ArrayImpl< ArrayStorageExt<Ty>      > { };
    template <typename Ty>                 struct Array    : ArrayImpl< ArrayStorage<Ty>         > { };
    template <typename Ty>                 struct ArrayH   : ArrayExt<Ty> { AllocatorI* m_allocator; };

    template <typename Ty, uint32_t MaxTy> struct ObjArrayT   : ObjArrayImpl< ArrayStorageT<Ty, MaxTy> > { };
    template <typename Ty>                 struct ObjArrayExt : ObjArrayImpl< ArrayStorageExt<Ty>      > { };
    template <typename Ty>                 struct ObjArray    : ObjArrayImpl< ArrayStorage<Ty>         > { };
    template <typename Ty>                 struct ObjArrayH   : ObjArrayExt<Ty> { AllocatorI* m_allocator; };

} // namespace DM_NAMESPACE
#   endif // DM_ARRAY_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
