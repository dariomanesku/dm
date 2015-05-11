/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_COMPILETIME_H_HEADER_GUARD
#define DM_COMPILETIME_H_HEADER_GUARD

#include <stdint.h>
#include <limits.h>  // CHAR_MAX

#ifndef DM_CPP11
#   define DM_CPP11 (__cplusplus >= 201103L)
#endif

namespace dm
{
    /// Log.
    /// Usage: Log<2, 512>::value
    template <uint8_t Base, uint32_t N>
    struct Log
    {
        enum { value = 1 + Log<Base, N/Base>::value };
    };
    template <uint8_t Base> struct Log<Base, 1> { enum { value = 0 }; };
    template <uint8_t Base> struct Log<Base, 0> { enum { value = 0 }; };

    /// Type info.
    /// Usage: uint8_t val = TyInfo<uint8_t>::Max();
    template <typename Ty> struct TyInfo { static bool      Max() { return false;       } };
    template <> struct TyInfo<bool>      { static bool      Max() { return true;        } };
    template <> struct TyInfo<char>      { static char      Max() { return CHAR_MAX;    } };
    template <> struct TyInfo<uint8_t>   { static uint8_t   Max() { return UINT8_MAX;   } };
    template <> struct TyInfo<uint16_t>  { static uint16_t  Max() { return UINT16_MAX;  } };
    template <> struct TyInfo<uint32_t>  { static uint32_t  Max() { return UINT32_MAX;  } };
    template <> struct TyInfo<uint64_t>  { static uint64_t  Max() { return UINT64_MAX;  } };
    template <> struct TyInfo<int8_t>    { static int8_t    Max() { return INT8_MAX;    } };
    template <> struct TyInfo<int16_t>   { static int16_t   Max() { return INT16_MAX;   } };
    template <> struct TyInfo<int32_t>   { static int32_t   Max() { return INT32_MAX;   } };
    template <> struct TyInfo<int64_t>   { static int64_t   Max() { return INT64_MAX;   } };
    template <> struct TyInfo<void*>     { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<bool*>     { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<char*>     { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<uint8_t*>  { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<uint16_t*> { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<uint32_t*> { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<uint64_t*> { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<int8_t*>   { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<int16_t*>  { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<int32_t*>  { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<int64_t*>  { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<float*>    { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<double*>   { static uintptr_t Max() { return UINTPTR_MAX; } };
    template <> struct TyInfo<float>     { static float     Max() { union { uint32_t m_u32; float  m_f; } val; val.m_u32 = UINT32_MAX; return val.m_f; } };
    template <> struct TyInfo<double>    { static double    Max() { union { uint64_t m_u64; double m_d; } val; val.m_u64 = UINT64_MAX; return val.m_d; } };

    /// Bool types.
    template <bool ValueT> struct bool_type { static const bool value = ValueT; };
    struct false_type : dm::bool_type<false> {};
    struct true_type  : dm::bool_type<true>  {};

    /// Remove cv.
    template <typename Ty> struct remove_const                 { typedef Ty type; };
    template <typename Ty> struct remove_const<const Ty>       { typedef Ty type; };
    template <typename Ty> struct remove_volatile              { typedef Ty type; };
    template <typename Ty> struct remove_volatile<volatile Ty> { typedef Ty type; };
    template <typename Ty> struct remove_cv { typedef typename dm::remove_volatile<typename dm::remove_const<Ty>::type>::type type; };

    /// Remove pointer.
    template <typename Ty> struct remove_pointer                     { typedef Ty type; };
    template <typename Ty> struct remove_pointer<Ty*>                { typedef Ty type; };
    template <typename Ty> struct remove_pointer<Ty* const>          { typedef Ty type; };
    template <typename Ty> struct remove_pointer<Ty* volatile>       { typedef Ty type; };
    template <typename Ty> struct remove_pointer<Ty* const volatile> { typedef Ty type; };

    /// Remove extent.
    template <typename Ty>             struct remove_extent        { typedef Ty type; };
    template <typename Ty>             struct remove_extent<Ty[]>  { typedef Ty type; };
    template <typename Ty, uint32_t N> struct remove_extent<Ty[N]> { typedef Ty type; };

    /// Remove all extents.
    template <typename Ty>             struct remove_all_extents        { typedef Ty type; };
    template <typename Ty>             struct remove_all_extents<Ty[]>  { typedef typename remove_all_extents<Ty>::type type; };
    template <typename Ty, uint32_t N> struct remove_all_extents<Ty[N]> { typedef typename remove_all_extents<Ty>::type type; };

    /// Naked type. Remove all extents and pointers.
    template <typename Ty>             struct naked_type                     { typedef Ty type; };
    template <typename Ty>             struct naked_type<Ty*>                { typedef Ty type; };
    template <typename Ty>             struct naked_type<Ty* const>          { typedef Ty type; };
    template <typename Ty>             struct naked_type<Ty* volatile>       { typedef Ty type; };
    template <typename Ty>             struct naked_type<Ty* const volatile> { typedef Ty type; };
    template <typename Ty>             struct naked_type<Ty[]>               { typedef typename naked_type<Ty>::type type; };
    template <typename Ty, uint32_t N> struct naked_type<Ty[N]>              { typedef typename naked_type<Ty>::type type; };

    /// Is same.
    template <typename Ty, typename U> struct is_same         : dm::false_type {};
    template <typename Ty>             struct is_same<Ty, Ty> : dm::true_type  {};

    /// C types.
    template <typename Ty> struct is_bool                   : dm::bool_type<dm::is_same<bool,                    typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_char                   : dm::bool_type<dm::is_same<char,                    typename dm::remove_cv<Ty>::type>::value> {};
    #if DM_CPP11
    template <typename Ty> struct is_char16                 : dm::bool_type<dm::is_same<char16_t,                typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_char32                 : dm::bool_type<dm::is_same<char32_t,                typename dm::remove_cv<Ty>::type>::value> {};
    #endif //DM_CPP11
    template <typename Ty> struct is_wchar                  : dm::bool_type<dm::is_same<wchar_t,                 typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_signed_char            : dm::bool_type<dm::is_same<signed char,             typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_short_int              : dm::bool_type<dm::is_same<short int,               typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_int                    : dm::bool_type<dm::is_same<int,                     typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_long_int               : dm::bool_type<dm::is_same<long int,                typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_long_long_int          : dm::bool_type<dm::is_same<long long int,           typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_unsigned_char          : dm::bool_type<dm::is_same<unsigned char,           typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_unsigned_short_int     : dm::bool_type<dm::is_same<unsigned short int,      typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_unsigned_int           : dm::bool_type<dm::is_same<unsigned int,            typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_unsigned_long_int      : dm::bool_type<dm::is_same<unsigned long int,       typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_unsigned_long_long_int : dm::bool_type<dm::is_same<unsigned long long int,  typename dm::remove_cv<Ty>::type>::value> {};

    /// Stdint types.
    template <typename Ty> struct is_uint8  : dm::bool_type<dm::is_same<uint8_t,  typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_uint16 : dm::bool_type<dm::is_same<uint16_t, typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_uint32 : dm::bool_type<dm::is_same<uint32_t, typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_uint64 : dm::bool_type<dm::is_same<uint64_t, typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_int8   : dm::bool_type<dm::is_same<int8_t,   typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_int16  : dm::bool_type<dm::is_same<int16_t,  typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_int32  : dm::bool_type<dm::is_same<int32_t,  typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_int64  : dm::bool_type<dm::is_same<int64_t,  typename dm::remove_cv<Ty>::type>::value> {};

    /// Floating point types.
    template <typename Ty> struct is_float       : dm::bool_type<dm::is_same<float,       typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_double      : dm::bool_type<dm::is_same<double,      typename dm::remove_cv<Ty>::type>::value> {};
    template <typename Ty> struct is_long_double : dm::bool_type<dm::is_same<long double, typename dm::remove_cv<Ty>::type>::value> {};

    /// Is array.
    /// Usage: bool val = dm::is_array<int[]>::value
    template <typename Ty>             struct is_array        : dm::false_type {};
    template <typename Ty>             struct is_array<Ty[]>  : dm::true_type  {};
    template <typename Ty, uint32_t N> struct is_array<Ty[N]> : dm::true_type  {};

    /// Is character.
    /// Usage: bool val = dm::is_character<char>::value
    template <typename Ty> struct is_character : dm::bool_type <dm::is_char<Ty>::value
                                                              #if DM_CPP11
                                                              ||dm::is_char16<Ty>::value
                                                              ||dm::is_char32<Ty>::value
                                                              #endif //DM_CPP11
                                                              ||dm::is_wchar<Ty>::value
                                                              > {};

    /// Is integral.
    /// Usage: bool val = dm::is_integral<int>::value
    template <typename Ty> struct is_integral : dm::bool_type <dm::is_bool<Ty>::value
                                                             ||dm::is_char<Ty>::value
                                                             #if DM_CPP11
                                                             ||dm::is_char16<Ty>::value
                                                             ||dm::is_char32<Ty>::value
                                                             #endif //DM_CPP11
                                                             ||dm::is_wchar<Ty>::value
                                                             ||dm::is_signed_char<Ty>::value
                                                             ||dm::is_short_int<Ty>::value
                                                             ||dm::is_int<Ty>::value
                                                             ||dm::is_long_int<Ty>::value
                                                             ||dm::is_long_long_int<Ty>::value
                                                             ||dm::is_unsigned_char<Ty>::value
                                                             ||dm::is_unsigned_short_int<Ty>::value
                                                             ||dm::is_unsigned_int<Ty>::value
                                                             ||dm::is_unsigned_long_int<Ty>::value
                                                             ||dm::is_unsigned_long_long_int<Ty>::value
                                                              > {};

    /// Is floating point.
    /// Usage: bool val = dm::is_floating_point<float>::value
    template <typename Ty>
    struct is_floating_point : dm::bool_type<dm::is_float<Ty>::value || dm::is_double<Ty>::value || dm::is_long_double<Ty>::value> {};

    /// Is pointer.
    /// Usage: bool val = dm::is_pointer<void*>::value
    template <typename Ty> struct testIsPointer      : dm::false_type {};
    template <typename Ty> struct testIsPointer<Ty*> : dm::true_type {};
    template <typename Ty> struct is_pointer : testIsPointer<typename dm::remove_cv<Ty>::type> {};

    /// Is member pointer.
    /// Usage: bool val = dm::is_member_pointer<int(Foo::*)>::value
    template <typename Ty>             struct testIsMemberPointer          : dm::false_type {};
    template <typename Ty, typename U> struct testIsMemberPointer<Ty U::*> : dm::true_type {};
    template <typename Ty>             struct is_member_pointer : testIsMemberPointer<typename dm::remove_cv<Ty>::type> {};

    /// Is arithmetic.
    /// Usage: bool val = dm::is_arithmetic<int32_t>::value
    template <typename Ty> struct is_arithmetic : dm::bool_type <dm::is_integral<Ty>::value
                                                               ||dm::is_float<Ty>::value
                                                               > {};

    /// Is scalar.
    /// Usage: bool val = dm::is_scalar<void*>::value
    template <typename Ty> struct is_scalar : dm::bool_type <dm::is_arithmetic<Ty>::value
                                                           ||dm::is_pointer<Ty>::value
                                                           ||dm::is_member_pointer<Ty>::value
                                                            > {};
    /// Is char array/ptr/string.
    /// Usage: bool val = dm::is_char_array<char[]>::value
    /// Usage: bool val = dm::is_char_ptr<char*>::value
    /// Usage: bool val = dm::is_char_string<char*>::value
    template <typename Ty> struct is_char_array  : dm::bool_type <dm::is_array<Ty>::value   && dm::is_character<typename dm::naked_type<Ty>::type>::value > {};
    template <typename Ty> struct is_char_ptr    : dm::bool_type <dm::is_pointer<Ty>::value && dm::is_character<typename dm::naked_type<Ty>::type>::value > {};
    template <typename Ty> struct is_char_string : dm::bool_type <dm::is_char_array<Ty>::value || dm::is_char_ptr<Ty>::value > {};

    /// Size test.
    /// Usage: bool val = dm::is_32bit<float>::value
    template <typename Ty> struct is_8bit  : dm::bool_type <sizeof(Ty) == 1> {};
    template <typename Ty> struct is_16bit : dm::bool_type <sizeof(Ty) == 2> {};
    template <typename Ty> struct is_32bit : dm::bool_type <sizeof(Ty) == 4> {};
    template <typename Ty> struct is_64bit : dm::bool_type <sizeof(Ty) == 8> {};

    /// Is power of two.
    /// Usage: bool val = dm::is_powtwo<float>::value
    #ifndef DM_IS_POW_TWO
    #   define DM_IS_POW_TWO(_v) ((0!=(_v)) && (0==((_v)&((_v)-1))))
    #endif // DM_IS_POW_TWO
    template <uint32_t Val> struct is_powtwo : dm::bool_type <DM_IS_POW_TWO(Val)> {};

    /// Is class.
    /// Usage: bool val = dm::is_class<Foo>::value
    template <typename Ty> char testIsClass(int Ty::*);
    struct sizeOfTwo { char c[2]; }; template <typename Ty> sizeOfTwo testIsClass(...);
    template <typename Ty> struct is_class : dm::bool_type<sizeof(testIsClass<Ty>(0))==1> {};

    /// Enable if.
    template <bool B, typename Ty = void> struct enable_if {};
    template <typename Ty> struct enable_if<true, Ty> { typedef Ty type; };

    /// Usage:
    ///     template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic)>
    ///     void func(Ty _val) { /*...*/ }
    /// or
    ///     template <uint32_t MaxT_PowTwo, DM_ENABLE_IF(MaxT_PowTwo, is_powtwo)>
    ///     struct Foo { };
    #define DM_ENABLE_IF(_templateParam, _testFunc) typename dm::enable_if<dm::_testFunc<_templateParam>::value == true, void>::type* = nullptr

} // namespace dm

#endif // DM_COMPILETIME_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
