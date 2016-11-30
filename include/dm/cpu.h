/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include "platform.h"

    #if DM_CPP11
    #   include <thread>
    #endif // DM_CPP11

    #if DM_PLATFORM_WINDOWS
    #   include <windows.h>
    #elif DM_PLATOFORM_LINUX
    #   include <unistd.h>
    #elif DM_PLATOFORM_APPLE
    #   include <sys/sysctl.h>
    #endif // DM_PLATFORM_*
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_CPU_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_CPU_H_HEADER_GUARD
#   define DM_CPU_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    // https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
    int numCpuThreads()
    {
        #if DM_PLATFORM_WINDOWS
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            int numCpu = sysinfo.dwNumberOfProcessors;
            return numCpu;
        #elif DM_PLATOFORM_LINUX
            int numCpu = (int)sysconf(_SC_NPROCESSORS_ONLN);
            return numCpu;
        #elif DM_PLATOFORM_APPLE
            int mib[4];
            int numCPU;
            std::size_t len = sizeof(numCPU);

            /* Set the mib for hw.ncpu. */
            mib[0] = CTL_HW;
            mib[1] = HW_AVAILCPU;  // Alternatively, try HW_NCPU.

            /* Get the number of CPUs from the system. */
            sysctl(mib, 2, &numCPU, &len, NULL, 0);

            if (numCPU < 1)
            {
                mib[1] = HW_NCPU;
                sysctl(mib, 2, &numCPU, &len, NULL, 0);
                if (numCPU < 1)
                    numCPU = 1;
            }
            return numCpu;
        #elif DM_CPP11
            int numCpu = (int)std::thread::hardware_concurrency();
            return numCpu;
        #else
            return 0;
        #endif // DM_PLATFORM_WINDOWS
    }

} // namespace DM_NAMESPACE
#   endif // DM_CPU_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/* vim: set sw=4 ts=4 expandtab: */
