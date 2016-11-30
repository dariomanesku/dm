/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

/*
 * Adapted from: https://github.com/bkaradzic/bx/include/bx/thread.h
 * Copyright 2010-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #if DM_PLATFORM_POSIX
    #   include <pthread.h>
    #   if defined(__FreeBSD__)
    #       include <pthread_np.h>
    #   endif // defined(__FreeBSD__)
    #   if DM_PLATFORM_LINUX && (DM_CRT_GLIBC < 21200)
    #       include <sys/prctl.h>
    #   endif // DM_PLATFORM_
    #elif DM_PLATFORM_WINDOWS
    #   include <errno.h>
    #endif // DM_PLATFORM_

    #if DM_PLATFORM_LINUX || DM_PLATFORM_APPLE
    #   include <pthread.h>
    #elif DM_PLATFORM_WINDOWS
    #   include <errno.h>
    #endif // DM_PLATFORM_
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_THREAD_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_THREAD_H_HEADER_GUARD
#   define DM_THREAD_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    #if DM_PLATFORM_WINDOWS
        typedef CRITICAL_SECTION pthread_mutex_t;
        typedef unsigned pthread_mutexattr_t;

        inline int pthread_mutex_lock(pthread_mutex_t* _mutex)
        {
            EnterCriticalSection(_mutex);
            return 0;
        }

        inline int pthread_mutex_unlock(pthread_mutex_t* _mutex)
        {
            LeaveCriticalSection(_mutex);
            return 0;
        }

        inline int pthread_mutex_trylock(pthread_mutex_t* _mutex)
        {
            return TryEnterCriticalSection(_mutex) ? 0 : EBUSY;
        }

        inline int pthread_mutex_init(pthread_mutex_t* _mutex, pthread_mutexattr_t* /*_attr*/)
        {
            InitializeCriticalSection(_mutex);
            return 0;
        }

        inline int pthread_mutex_destroy(pthread_mutex_t* _mutex)
        {
            DeleteCriticalSection(_mutex);
            return 0;
        }
    #endif // DM_PLATFORM_

    struct Mutex
    {
        Mutex()
        {
            pthread_mutexattr_t attr;
            #if DM_PLATFORM_WINDOWS
            #else
                pthread_mutexattr_init(&attr);
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            #endif // DM_PLATFORM_WINDOWS
            pthread_mutex_init(&m_handle, &attr);
        }

        ~Mutex()
        {
            pthread_mutex_destroy(&m_handle);
        }

        void lock()
        {
            pthread_mutex_lock(&m_handle);
        }

        void unlock()
        {
            pthread_mutex_unlock(&m_handle);
        }

    private:
        pthread_mutex_t m_handle;
    };

    struct MutexScope
    {
        MutexScope(Mutex& _mutex)
            : m_mutex(_mutex)
        {
            m_mutex.lock();
        }

        ~MutexScope()
        {
            m_mutex.unlock();
        }

    private:
        Mutex& m_mutex;
    };

    typedef Mutex LwMutex;

    struct LwMutexScope
    {
        LwMutexScope(LwMutex& _mutex)
            : m_mutex(_mutex)
        {
            m_mutex.lock();
        }

        ~LwMutexScope()
        {
            m_mutex.unlock();
        }

    private:
        LwMutex& m_mutex;
    };
} // namespace DM_NAMESPACE
#   endif // DM_H_HEADER_GUARD
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
//
// ... impl body.
//
} // namespace DM_NAMESPACE
#endif // (DM_INCL & DM_INCL_IMPL_BODY)

/* vim: set sw=4 ts=4 expandtab: */
