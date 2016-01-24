/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

/*
================================================================================

  You MUST

      Define desired flags before including this file.
          #define DM_DEFINE_TYPES
          #define DM_UNDEFINE_TYPES

  Contents (SSS):
      SECTION S10 "Define types"
      SECTION S20 "Undefine types"

================================================================================
*/

#ifdef DM_DEFINE_TYPES
#   define DM_DEFINE_TYPES_IMPL 1
#else
#   define DM_DEFINE_TYPES_IMPL 0
#endif

#ifdef DM_UNDEFINE_TYPES
#   define DM_UNDEFINE_TYPES_IMPL 1
#else
#   define DM_UNDEFINE_TYPES_IMPL 0
#endif

//------------------------------------------------------------
// SSS{ SECTION S10 "Define types"
//------------------------------------------------------------
#if DM_DEFINE_TYPES_IMPL
#ifndef DM_NG_TYPES_HEADER_GUARD
#define DM_NG_TYPES_HEADER_GUARD

#include <inttypes.h>
#include <float.h>

#define u8   uint8_t
#define u8c  uint8_t const
#define i8   int8_t
#define i8c  int8_t const

#define u16  uint16_t
#define u16c uint16_t const
#define i16  int16_t
#define i16c int16_t const

#define u32  uint32_t
#define u32c uint32_t const
#define i32  int32_t
#define i32c int32_t const

#define u64  uint64_t
#define u64c uint64_t const
#define i64  int64_t
#define i64c int64_t const

#define r32  float
#define r32c float const
#define r64  double
#define r64c double const

#endif // DM_NG_TYPES_HEADER_GUARD
#endif // DM_DEFINE_TYPES_IMPL
//------------------------------------------------------------
// }SSS ENDSEC S10 "Define types"
//------------------------------------------------------------

//------------------------------------------------------------
// SSS{ SECTION S20 "Undefine types"
//------------------------------------------------------------
#if DM_UNDEFINE_TYPES_IMPL
#ifndef DM_NG_TYPES_HEADER_GUARD
#define DM_NG_TYPES_HEADER_GUARD

#undef u8
#undef u8c
#undef i8
#undef i8c

#undef u16
#undef u16c
#undef i16
#undef i16c

#undef u32
#undef u32c
#undef i32
#undef i32c

#undef r32
#undef r32c
#undef r64
#undef r64c

#endif // DM_NG_TYPES_HEADER_GUARD
#endif // DM_UNDEFINE_TYPES_IMPL
//------------------------------------------------------------
// }SSS ENDSEC S20 "Define types"
//------------------------------------------------------------

/* vim: set sw=4 ts=4 expandtab: */
