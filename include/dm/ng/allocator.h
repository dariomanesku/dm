/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_ALLOCATOR_HEADER_GUARD
#define DM_NG_ALLOCATOR_HEADER_GUARD

#include <stdlib.h> // malloc(),realloc(),free().

namespace dm { namespace ng {

typedef void* (*AllocFunc)(size_t _size);
typedef void* (*ReallocFunc)(void* _ptr, size_t _size);
typedef void  (*FreeFunc)(void* _ptr);

struct Allocator
{
    AllocFunc m_allocFunc;
    FreeFunc  m_freeFunc;
};

struct Reallocator : Allocator
{
    ReallocFunc m_reallocFunc;
};

struct CrtAllocator : Allocator
{
    CrtAllocator()
    {
        m_allocFunc = &::malloc;
        m_freeFunc  = &::free;
    }
};

struct CrtReallocator : Reallocator
{
    CrtReallocator()
    {
        m_allocFunc   = &::malloc;
        m_freeFunc    = &::free;
        m_reallocFunc = &::realloc;
    }
};

} //namespace ng
} //namespace dm

#endif //DM_NG_ALLOCATOR_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
