/*
 * Copyright 2014-2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_CHECK_H_HEADER_GUARD
#define DM_CHECK_H_HEADER_GUARD

// Choose desired config.
//-----

#define DM_CHECK_CONFIG_NOOP        0
#define DM_CHECK_CONFIG_PRINT       1
#define DM_CHECK_CONFIG_DEBUG_BREAK 2

#ifndef DM_CHECK_CONFIG
#   define DM_CHECK_CONFIG DM_CHECK_CONFIG_NOOP
#endif //DM_CHECK_CONFIG

// Public flags.
//-----

#define DM_DEBUG_BUILD (DM_CHECK_CONFIG != DM_CHECK_CONFIG_NOOP)

// Implementation.
//-----

#define DM_CHECK_CONCATENATE(_x, _y) DM_CHECK_CONCATENATE_(_x, _y)
#define DM_CHECK_CONCATENATE_(_x, _y) _x ## _y

#define DM_CHECK_FILE_LINE "" __FILE__ "(" DM_STRINGIZE(__LINE__) ")"

#define _DM_CHECK_NOOP_IMPL(_condition, _format, ...) for (;;) { break; }
#define _DM_CHECK_PRINT_IMPL(_condition, _format, ...) \
    do                                                 \
    {                                                  \
        if (!(_condition))                             \
        {                                              \
            fprintf(stderr, "DM ERROR [" DM_CHECK_FILE_LINE "]: "  _format "\n", ##__VA_ARGS__); \
        }                                              \
    } while(0)
#define _DM_CHECK_BREAK_IMPL(_condition, _format, ...) \
    do                                                 \
    {                                                  \
        if (!(_condition))                             \
        {                                              \
            fprintf(stderr, "DM ERROR [" DM_CHECK_FILE_LINE "]: "  _format "\n", ##__VA_ARGS__); \
            dm::ng::debugBreak();                      \
        }                                              \
    } while(0)

// Setup.
//-----

#if !defined(DM_DEBUG_EVAL)
#   if DM_DEBUG_BUILD
#       define DM_DEBUG_EVAL(_expr) _expr
#   else
#       define DM_DEBUG_EVAL(_expr)
#   endif //DM_DEBUG_BUILD
#endif // !defined(DM_DEBUG_EVAL)

#if !defined(DM_CHECK)
#   if (DM_CHECK_CONFIG == DM_CHECK_CONFIG_PRINT)
#       define DM_CHECK _DM_CHECK_PRINT_IMPL
#       include <stdio.h> // fprintf()
#   elif (DM_CHECK_CONFIG == DM_CHECK_CONFIG_DEBUG_BREAK)
#       define DM_CHECK _DM_CHECK_BREAK_IMPL
#       include <stdio.h> // fprintf()
#       include "ng/debug.h" // dm::ng::debugBreak()
#   else
#       define DM_CHECK _DM_CHECK_NOOP_IMPL
#   endif //(DM_CHECK_CONFIG == DM_CHECK_CONFIG_PRINT)
#endif //!defined(DM_CHECK)

#if !defined(DM_ASSERT)
#    define DM_ASSERT(_condition) DM_CHECK(_condition, "Assertion failed!")
#endif //!defined(DM_ASSERT)

#endif // DM_CHECK_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
