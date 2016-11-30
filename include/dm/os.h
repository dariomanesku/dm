/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include "platform.h"

    #if DM_PLATFORM_WINDOWS || DM_PLATFORM_WINRT
    #   include <windows.h>
    #elif DM_PLATFORM_LINUX || DM_PLATFORM_APPLE
    #   include <sched.h> // sched_yield
    #   if DM_PLATFORM_APPLE
    #       include <pthread.h> // mach_port_t
    #   endif // DM_PLATFORM_*
    #
    #   include <time.h> // nanosleep
    #   include <dlfcn.h> // dlopen, dlclose, dlsym
    #
    #   if DM_PLATFORM_LINUX
    #       include <unistd.h> // syscall
    #       include <sys/syscall.h>
    #   endif // DM_PLATFORM_LINUX
    #endif // DM_PLATFORM_

    #if DM_COMPILER_MSVC
    #   include <direct.h> // _getcwd
    #else
    #   include <unistd.h> // getcwd
    #endif // DM_COMPILER_MSVC
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_OS_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_OS_H_HEADER_GUARD
#   define DM_OS_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    DM_INLINE void setenv(const char* _name, const char* _value)
    {
        #if DM_PLATFORM_WINDOWS
            ::SetEnvironmentVariableA(_name, _value);
        #else
            ::setenv(_name, _value, 1);
        #endif // DM_PLATFORM_
    }

    DM_INLINE void unsetenv(const char* _name)
    {
        #if DM_PLATFORM_WINDOWS
            ::SetEnvironmentVariableA(_name, NULL);
        #else
            ::unsetenv(_name);
        #endif // DM_PLATFORM_
    }

    DM_INLINE int chdir(const char* _path)
    {
        #if DM_COMPILER_MSVC
            return ::_chdir(_path);
        #else
            return ::chdir(_path);
        #endif // DM_COMPILER_
    }

    DM_INLINE char* pwd(char* _buffer, uint32_t _size)
    {
        #if DM_COMPILER_MSVC
            return ::_getcwd(_buffer, (int)_size);
        #else
            return ::getcwd(_buffer, _size);
        #endif // DM_COMPILER_
    }

} // namespace DM_NAMESPACE
#   endif // DM_OS_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
