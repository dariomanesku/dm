/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_ALLOCATOR_PRIVATE_H_HEADER_GUARD
#define DM_ALLOCATOR_PRIVATE_H_HEADER_GUARD

#include <stdio.h>

#if DM_ALLOC_PRINT_FILELINE
    #define DM_ALLOC_FILE_LINE BX_FILE_LINE_LITERAL
#else
    #define DM_ALLOC_FILE_LINE
#endif //DM_ALLOC_PRINT_FILELINE

#define DM_ALLOC_PRINT(_format, ...) do { fprintf(stderr, "CS MEM " DM_ALLOC_FILE_LINE "" _format "\n", ##__VA_ARGS__); } while(0)

#if DM_ALLOC_PRINT_STATS
    #define DM_PRINT_MEM_STATS(_format, ...) DM_ALLOC_PRINT(_format, __VA_ARGS__)
#else
    #define DM_PRINT_MEM_STATS(...)
#endif //DM_ALLOC_PRINT_STATS

#if DM_ALLOC_PRINT_STATIC
    #define DM_PRINT_STATIC(_format, ...) DM_ALLOC_PRINT(_format, __VA_ARGS__)
#else
    #define DM_PRINT_STATIC(...)
#endif //DM_ALLOC_PRINT_STATIC

#if DM_ALLOC_PRINT_SMALL
    #define DM_PRINT_SMALL(_format, ...) DM_ALLOC_PRINT(_format, __VA_ARGS__)
#else
    #define DM_PRINT_SMALL(...)
#endif //DM_ALLOC_PRINT_SMALL

#if DM_ALLOC_PRINT_STACK
    #define DM_PRINT_STACK(_format, ...) DM_ALLOC_PRINT(_format, __VA_ARGS__)
#else
    #define DM_PRINT_STACK(...)
#endif //DM_ALLOC_PRINT_STACK

#if DM_ALLOC_PRINT_HEAP
    #define DM_PRINT_HEAP(_format, ...) DM_ALLOC_PRINT(_format, __VA_ARGS__)
#else
    #define DM_PRINT_HEAP(...)
#endif //DM_ALLOC_PRINT_HEAP

#if DM_ALLOC_PRINT_EXT
    #define DM_PRINT_EXT(_format, ...) DM_ALLOC_PRINT(_format, __VA_ARGS__)
#else
    #define DM_PRINT_EXT(...)
#endif //DM_ALLOC_PRINT_EXT

#if DM_ALLOC_PRINT_BGFX
    #define DM_PRINT_BGFX(_format, ...) DM_ALLOC_PRINT(_format, __VA_ARGS__)
#else
    #define DM_PRINT_BGFX(...)
#endif //DM_ALLOC_PRINT_BGFX

#endif // DM_ALLOCATOR_PRIVATE_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
