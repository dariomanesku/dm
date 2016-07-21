/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_COMPILETIME_H_HEADER_GUARD
#define DM_COMPILETIME_H_HEADER_GUARD

#include <stdint.h>
#include <limits.h>  // CHAR_MAX

#include "ng/platform.h" // DM_CPP11

namespace dm
{
    /// Static assert.
    /// To be used as a statement inside a scope.
    /// Example: template <typename Ty> void foo(Ty _in) { dm_staticAssert(sizeof(Ty) == 4); }
    template <bool> struct StaticAssert;               // Declaration.
    template     <> struct StaticAssert<true> {};      // True has a definition.
    #define dm_staticAssert(x) dm::StaticAssert<(x)>() // False will cause compile-time error.

    /// Log.
    /// Usage: Log<2, 512>::value
    template <uint8_t Base, uint32_t N>
    struct Log
    {
        enum { value = 1 + Log<Base, N/Base>::value };
    };
    template <uint8_t Base> struct Log<Base, 1> { enum { value = 0 }; };
    template <uint8_t Base> struct Log<Base, 0> { enum { value = 0 }; };

    /// Pow.
    /// Usage: Pow<2, 16>::value
    template <uint8_t Base, uint32_t N>
    struct Pow
    {
        enum { value = Base * Pow<Base, N-1>::value };
    };
    template <uint8_t Base> struct Pow<Base, 0> { enum { value = 1 }; };

    /// Next Pow Two.
    template <uint32_t Val>
    struct NextPowTwo
    {
        enum { value = 1 << (dm::Log<2, Val-1>::value + 1) };
    };

    /// Type info.
    /// Usage: uint8_t val = TyInfo<uint8_t>::Max();
    /// Alternatively: uint8_t val = TyInfo<uint8_t>::MaxVal;
    template <typename Ty> struct TyInfo { static const bool      max = false;       static bool      Max() { return max; } };
    template <> struct TyInfo<bool>      { static const bool      max = true;        static bool      Max() { return max; } };
    template <> struct TyInfo<char>      { static const char      max = CHAR_MAX;    static char      Max() { return max; } };
    template <> struct TyInfo<uint8_t>   { static const uint8_t   max = UINT8_MAX;   static uint8_t   Max() { return max; } };
    template <> struct TyInfo<uint16_t>  { static const uint16_t  max = UINT16_MAX;  static uint16_t  Max() { return max; } };
    template <> struct TyInfo<uint32_t>  { static const uint32_t  max = UINT32_MAX;  static uint32_t  Max() { return max; } };
    template <> struct TyInfo<uint64_t>  { static const uint64_t  max = UINT64_MAX;  static uint64_t  Max() { return max; } };
    template <> struct TyInfo<int8_t>    { static const int8_t    max = INT8_MAX;    static int8_t    Max() { return max; } };
    template <> struct TyInfo<int16_t>   { static const int16_t   max = INT16_MAX;   static int16_t   Max() { return max; } };
    template <> struct TyInfo<int32_t>   { static const int32_t   max = INT32_MAX;   static int32_t   Max() { return max; } };
    template <> struct TyInfo<int64_t>   { static const int64_t   max = INT64_MAX;   static int64_t   Max() { return max; } };
    template <> struct TyInfo<void*>     { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<bool*>     { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<char*>     { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<uint8_t*>  { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<uint16_t*> { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<uint32_t*> { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<uint64_t*> { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<int8_t*>   { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<int16_t*>  { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<int32_t*>  { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<int64_t*>  { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<float*>    { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
    template <> struct TyInfo<double*>   { static const uintptr_t max = UINTPTR_MAX; static uintptr_t Max() { return max; } };
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

    /// Type size test.
    /// Usage: bool val = dm::is_32bit<float>::value
    template <typename Ty> struct is_8bit  : dm::bool_type <sizeof(Ty) == 1> {};
    template <typename Ty> struct is_16bit : dm::bool_type <sizeof(Ty) == 2> {};
    template <typename Ty> struct is_32bit : dm::bool_type <sizeof(Ty) == 4> {};
    template <typename Ty> struct is_64bit : dm::bool_type <sizeof(Ty) == 8> {};

    /// Type fit test.
    /// Usage: bool val = dm::fits_32bit<float>::value
    template <uint64_t Val> struct fits_8bit  : dm::bool_type <Val <= UINT8_MAX>  {};
    template <uint64_t Val> struct fits_16bit : dm::bool_type <Val <= UINT16_MAX> {};
    template <uint64_t Val> struct fits_32bit : dm::bool_type <Val <= UINT32_MAX> {};
    template <uint64_t Val> struct fits_64bit : dm::bool_type <Val <= UINT64_MAX> {};

    /// Type best fit test.
    /// Usage: bool val = dm::bestfit_16bit<500>::value
    template <uint64_t Val> struct bestfit_8bit  : dm::bool_type <         0 <= uint64_t(Val) && uint64_t(Val) <= UINT8_MAX > {};
    template <uint64_t Val> struct bestfit_16bit : dm::bool_type < UINT8_MAX  < uint64_t(Val) && uint64_t(Val) <= UINT16_MAX> {};
    template <uint64_t Val> struct bestfit_32bit : dm::bool_type <UINT16_MAX  < uint64_t(Val) && uint64_t(Val) <= UINT32_MAX> {};
    template <uint64_t Val> struct bestfit_64bit : dm::bool_type <UINT32_MAX  < uint64_t(Val) && uint64_t(Val) <= UINT64_MAX> {};

    /// Get best fit type.
    /// Usage: bestfit_type<567>::type foo; // 'foo' will be uint16_t.
    template <uint64_t Val> struct bestfit_type_impl    { typedef uint64_t type; };
    template             <> struct bestfit_type_impl<1> { typedef uint32_t type; };
    template             <> struct bestfit_type_impl<2> { typedef uint16_t type; };
    template             <> struct bestfit_type_impl<3> { typedef uint8_t  type; };
    template <uint64_t Val> struct bestfit_type
    {
        typedef typename bestfit_type_impl<(unsigned)dm::fits_32bit<Val>::value
                                          +(unsigned)dm::fits_16bit<Val>::value
                                          +(unsigned)dm::fits_8bit <Val>::value
                                          >::type type;
    };

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
    template <bool B, typename Ty> struct enable_if {};
    template <typename Ty> struct enable_if<true, Ty> { typedef Ty type; };

    /// Usage:
    ///     template <typename Ty, DM_ENABLE_IF(Ty, is_arithmetic)>
    ///     void func(Ty _val) { /*...*/ }
    /// or
    ///     template <uint32_t MaxT_PowTwo, DM_ENABLE_IF(MaxT_PowTwo, is_powtwo)>
    ///     struct Foo { };
    #define DM_ENABLE_IF(_templateParam, _testFunc) typename dm::enable_if<dm::_testFunc<_templateParam>::value == true, void>::type* = NULL

} // namespace dm

#endif // DM_COMPILETIME_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
