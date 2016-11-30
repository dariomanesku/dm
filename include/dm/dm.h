/*
 * Copyright 2014-2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

///
/// Always specify or undefine DM_INCL before including dm files.
///     DM_INCL options:
///         DM_INCL_HEADER_INCLUDES
///         DM_INCL_HEADER_BODY
///         DM_INCL_HEADER
///
///         DM_INCL_IMPL_INCLUDES
///         DM_INCL_IMPL_BODY
///         DM_INCL_IMPL
///
///         DM_INCL_INCLUDES
///         DM_INCL_ALL
///
///         DM_INCL_HEADER_OPT_REMOVE_HEADER_GUARD
///

#ifndef DM_DM_H_INCL_HEADER_GUARD
#   define DM_INCL_HEADER_INCLUDES 0x1
#   define DM_INCL_HEADER_BODY     0x2
#   define DM_INCL_HEADER (DM_INCL_HEADER_INCLUDES|DM_INCL_HEADER_BODY)

#   define DM_INCL_IMPL_INCLUDES 0x4
#   define DM_INCL_IMPL_BODY     0x8
#   define DM_INCL_IMPL (DM_INCL_IMPL_INCLUDES|DM_INCL_IMPL_BODY)

#   define DM_INCL_ALL      (DM_INCL_HEADER|DM_INCL_IMPL)
#   define DM_INCL_INCLUDES (DM_INCL_HEADER_INCLUDES|DM_INCL_IMPL_INCLUDES)
#   define DM_INCL_DEFAULT  (DM_INCL_HEADER)

#   define DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD 0x10
#endif // DM_DM_H_INCL_HEADER_GUARD

#ifndef DM_INCL
#   define DM_INCL DM_INCL_HEADER
#endif // DM_INCL

///
/// Overridable options:
///
#ifndef DM_NAMESPACE
#   define DM_NAMESPACE dm
#endif

#ifndef DM_INLINE
#   define DM_INLINE inline
#endif //DM_INLINE

/******************************
 * Template for dm files: *
 ******************************

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
//
// ... header includes.
//
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_H_HEADER_GUARD
#   define DM_H_HEADER_GUARD
namespace DM_NAMESPACE
{
//
// ... header body.
//
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

********************************/

/* vim: set sw=4 ts=4 expandtab: */
