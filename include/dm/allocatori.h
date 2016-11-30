/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <stdint.h> // uint16_t
    #include <stdlib.h> // ::realloc().
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_ALLOCATORI_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_ALLOCATORI_H_HEADER_GUARD
#   define DM_ALLOCATORI_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    #ifndef DM_ALLOCATOR_DEBUG
    #   define DM_ALLOCATOR_DEBUG 0
    #endif // DM_ALLOCATOR_DEBUG

    #ifndef DM_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
    #   define DM_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
    #endif // DM_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT

    #if defined(_MSC_VER)
    #   ifndef DM_NO_VTABLE
    #       define DM_NO_VTABLE __declspec(novtable)
    #   endif // DM_NO_VTABLE
    #else
    #   ifndef DM_NO_VTABLE
    #       define DM_NO_VTABLE
    #   endif // DM_NO_VTABLE
    #endif

    struct DM_NO_VTABLE AllocatorI
    {
        virtual void* realloc(void* _ptr, size_t _size, size_t _align, const char* _file, size_t _line) = 0;
    };

    struct DM_NO_VTABLE StackAllocatorI : AllocatorI
    {
        virtual void push(const char* _file, size_t _line) = 0;
        virtual void pop(const char* _file, size_t _line) = 0;
    };

    struct StackAllocatorScope
    {
        StackAllocatorScope(StackAllocatorI* _stackAlloc) : m_stack(_stackAlloc)
        {
            m_stack->push(0,0);
        }

        ~StackAllocatorScope()
        {
            m_stack->pop(0,0);
        }

    private:
        StackAllocatorI* m_stack;
    };

    #if DM_ALLOCATOR_DEBUG
    #   define DM_ALLOC(_allocator, _size)                         (_allocator)->realloc(NULL, _size,      0, __FILE__, __LINE__)
    #   define DM_REALLOC(_allocator, _ptr, _size)                 (_allocator)->realloc(_ptr, _size,      0, __FILE__, __LINE__)
    #   define DM_FREE(_allocator, _ptr)                           (_allocator)->realloc(_ptr,     0,      0, __FILE__, __LINE__)
    #   define DM_ALIGNED_ALLOC(_allocator, _size, _align)         (_allocator)->realloc(NULL, _size, _align, __FILE__, __LINE__)
    #   define DM_ALIGNED_REALLOC(_allocator, _ptr, _size, _align) (_allocator)->realloc(_ptr, _size, _align, __FILE__, __LINE__)
    #   define DM_ALIGNED_FREE(_allocator, _ptr, _align)           (_allocator)->realloc(_ptr,     0, _align, __FILE__, __LINE__)
    #   define DM_PUSH(_stackAllocator) (_stackAllocator)->push(__FILE__, __LINE__)
    #   define DM_POP(_stackAllocator)  (_stackAllocator)->pop(__FILE__, __LINE__)
    #else
    #   define DM_ALLOC(_allocator, _size)                         (_allocator)->realloc(NULL, _size,      0, 0, 0)
    #   define DM_REALLOC(_allocator, _ptr, _size)                 (_allocator)->realloc(_ptr, _size,      0, 0, 0)
    #   define DM_FREE(_allocator, _ptr)                           (_allocator)->realloc(_ptr,     0,      0, 0, 0)
    #   define DM_ALIGNED_ALLOC(_allocator, _size, _align)         (_allocator)->realloc(NULL, _size, _align, 0, 0)
    #   define DM_ALIGNED_REALLOC(_allocator, _ptr, _size, _align) (_allocator)->realloc(_ptr, _size, _align, 0, 0)
    #   define DM_ALIGNED_FREE(_allocator, _ptr, _align)           (_allocator)->realloc(_ptr,     0, _align, 0, 0)
    #   define DM_PUSH(_stackAllocator) (_stackAllocator)->push(0, 0)
    #   define DM_POP(_stackAllocator)  (_stackAllocator)->pop(0, 0)
    #endif // DM_ALLOCATOR_DEBUG

    struct CrtAllocator : AllocatorI
    {
        virtual void* realloc(void* _ptr, size_t _size, size_t _align, const char* /*_file*/, size_t /*_line*/)
        {
            (void)_align; // Ignoring alignment for now.

            if (0 == _ptr)
            {
                return ::malloc(_size);
            }
            else if (0 == _size)
            {
                ::free(_ptr);
                return NULL;
            }
            else
            {
                return ::realloc(_ptr, _size);
            }
        }
    };

    struct CrtStackAllocator : StackAllocatorI
    {
        virtual void* realloc(void* _ptr, size_t _size, size_t _align, const char* /*_file*/, size_t /*_line*/)
        {
            (void)_align; // Ignoring alignment for now.

            if (0 == _ptr)
            {
                void* ptr = ::malloc(_size);
                m_ptrs[m_curr++] = ptr;
                return ptr;
            }
            else if (0 == _size)
            {
                ::free(_ptr);
                return NULL;
            }
            else
            {
                void* ptr = ::realloc(_ptr, _size);
                m_ptrs[m_curr++] = ptr;
                return ptr;
            }
        }

        virtual void push(const char* /*_file*/, size_t /*_line*/)
        {
            m_frames[m_frameIdx++] = m_curr;
        }

        virtual void pop(const char* /*_file*/, size_t /*_line*/)
        {
            uint16_t prev = m_frames[--m_frameIdx];
            for (uint16_t ii = prev, iiEnd = m_curr; ii < iiEnd; ++ii)
            {
                ::free(m_ptrs[ii]);
            }
            m_curr = prev;
        }

        enum
        {
            MaxAllocations = 4096,
            MaxFrames      = 4096,
        };

        uint16_t m_curr;
        uint16_t m_frameIdx;
        void* m_ptrs[MaxAllocations];
        uint16_t m_frames[MaxAllocations];
    };

    extern CrtAllocator      g_crtAllocator;
    extern CrtStackAllocator g_crtStackAllocator;

} // namespace DM_NAMESPACE
#   endif // DM_ALLOCATORI_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/// Impl includes.
#if (DM_INCL & DM_INCL_IMPL_INCLUDES)
//
// ... impl includes.
//
#endif // (DM_INCL & DM_INCL_IMPL_INCLUDES)

/// Impl body.
#if (DM_INCL & DM_INCL_IMPL_BODY)
namespace DM_NAMESPACE
{
    CrtAllocator      g_crtAllocator;
    CrtStackAllocator g_crtStackAllocator;
} // namespace DM_NAMESPACE
#endif // (DM_INCL & DM_INCL_IMPL_BODY)

/* vim: set sw=4 ts=4 expandtab: */
