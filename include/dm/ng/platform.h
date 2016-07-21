/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_PLATFORM_HEADER_GUARD
#define DM_NG_PLATFORM_HEADER_GUARD

/// Pre-defined C/C++ Compiler Macros: http://sourceforge.net/p/predef/wiki/Home/

//------------------------------------------------------------
// Options.
//------------------------------------------------------------

#define DM_COMPILER_MSVC 0
#define DM_COMPILER_GCC 0
#define DM_COMPILER_CLANG 0

#define DM_ARCH_32BIT 0
#define DM_ARCH_64BIT 0

#define DM_PTR_SIZE 0

#define DM_PLATFORM_LINUX 0
#define DM_PLATFORM_APPLE 0
#define DM_PLATFORM_WINDOWS 0

#define DM_PLATFORM_UNIX  0
#define DM_PLATFORM_POSIX 0

#define DM_CPP11 0
#define DM_CPP14 0

//------------------------------------------------------------
// Impl.
//------------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)
#   undef DM_COMPILER_MSVC
#   define DM_COMPILER_MSVC 1
#elif defined(__GNUC__)
#   undef DM_COMPILER_GCC
#   define DM_COMPILER_GCC 1
#elif defined(__clang__)
#   undef DM_COMPILER_CLANG
#   define DM_COMPILER_CLANG 1
#endif

#if (0                      \
    || defined (__amd64__)  \
    || defined (__amd64)    \
    || defined (__x86_64__) \
    || defined (__x86_64)   \
    || defined (_M_X64)     \
    || defined (_M_AMD64)   )
#   undef DM_ARCH_64BIT
#   define DM_ARCH_64BIT 1
#   undef DM_PTR_SIZE
#   define DM_PTR_SIZE 8
#elif (0                   \
      || defined(i386)     \
      || defined(__i386)   \
      || defined(__i386__) \
      || defined(__i386)   \
      || defined(__IA32__) \
      || defined(_M_I86)   \
      || defined(_M_IX86)  \
      || defined(_X86_)    \
      || defined(__X86__)  )
#   undef DM_ARCH_32BIT
#   define DM_ARCH_32BIT 1
#   undef DM_PTR_SIZE
#   define DM_PTR_SIZE 4
#else
#   error Unsupported platform!
#endif

#if (0                        \
    || defined(__linux__)     \
    || defined(linux)         \
    || defined(__linux)       \
    || defined(__gnu_linux__) )
#   undef DM_PLATFORM_LINUX
#   define DM_PLATFORM_LINUX 1
#elif (0                    \
      || defined(__APPLE__) \
      || defined(macintosh) \
      || defined(Macintosh) )
#   undef DM_PLATFORM_APPLE
#   define DM_PLATFORM_APPLE 1
#elif (0 \
      || defined(_WIN16)      \
      || defined(_WIN32)      \
      || defined(_WIN64)      \
      || defined(__WIN32__)   \
      || defined(__TOS_WIN__) \
      || defined(__WINDOWS__) )
#   undef DM_PLATFORM_WINDOWS
#   define DM_PLATFORM_WINDOWS 1
#endif

#if defined(__unix__) || defined(__unix)
#   undef DM_PLATFORM_UNIX
#   define DM_PLATFORM_UNIX 1
#endif

#undef DM_PLATFORM_POSIX
#define DM_PLATFORM_POSIX (DM_PLATFORM_LINUX || DM_PLATFORM_APPLE || DM_PLATFORM_UNIX)

#undef DM_CPP11 0
#define DM_CPP11 (__cplusplus >= 201103L)
#undef DM_CPP14 0
#define DM_CPP14 (__cplusplus >= 201402L)

#endif //DM_NG_PLATFORM_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
