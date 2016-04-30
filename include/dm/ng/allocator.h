/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_ALLOCATOR_HEADER_GUARD
#define DM_NG_ALLOCATOR_HEADER_GUARD

#include <stdlib.h> // ::realloc().

namespace dm { namespace ng {

typedef void* (*ReallocFn)(void* _ptr, size_t _size);

static inline void* dm_alloc(size_t _size, ReallocFn _realloc = &::realloc)
{
    return _realloc(NULL, _size);
}

static inline void* dm_realloc(void* _ptr, size_t _size, ReallocFn _realloc = &::realloc)
{
    return _realloc(_ptr, _size);
}

static inline void* dm_free(void* _ptr, ReallocFn _realloc = &::realloc)
{
    return _realloc(_ptr, 0);
}

} //namespace ng
} //namespace dm

#endif //DM_NG_ALLOCATOR_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
