/* CEE - C Ease-of-use Extensions */
/* TODO:
 *  - could sizeof(**ptr) be used to enforce void ** checks:
 *      - #define free_ptr(ptr) free_ptr((sizeof(**ptr), ptr))
 *      - would this fail with a literal void ** rather than normal int **
 *      - usage: int *x = malloc(sizeof(int)); free_ptr(&x); // x now == 0
 *
 */

#ifndef CEE_H

// for data in global scope
#define global   static
// for data in function scope
#define persist  static
// TODO: change to "local"
// for functions
#define internal static
// for array function parameters
#define atleast static

// compile-time assert:
/* #define ct_assert(e) struct { char compile_time_assert[(e) ? 1 : -1]; } */
#define ct_assert(e) typedef char compile_time_assertion[(e) ? 1 : -1]

// INTERNAL TO HEADER //
#define CEE_CAT1(a,b) a ## b
#define CEE_CAT2(a,b) CEE_CAT1(a,b)
#define CEE_CAT(a,b)  CEE_CAT2(a,b)

#define cee_n CEE_CAT(cee_internal_n_, __LINE__)
#define cee_a CEE_CAT(cee_internal_a_, __LINE__)
#define cee_b CEE_CAT(cee_internal_b_, __LINE__)

#define CEE_FIRST(a, ...) a
#define CEE_FIRST_ONLY(...) CEE_FIRST(__VA_ARGS__, DUMMY)
#define CEE_REST(a, ...) __VA_ARGS__

#define CEE_SECOND(a, b, ...) b
#define CEE_HAS_MULTIPLE(...) CEE_SECOND((__VA_ARGS__), (DUMMY1), DUMMY2)
CEE_REST(__VA_ARGS__ )

#if 1 // TYPES //

#define cast(type, data) ((type)(data))
#define ptr_cast(type, data) ((type)(uintptr_t)(data))

#include <stdint.h>

// bytes //
typedef uint8_t  U1;
typedef uint16_t U2;
typedef uint32_t U4;
typedef uint64_t U8;

typedef  int8_t  I1;
typedef  int16_t I2;
typedef  int32_t I4;
typedef  int64_t I8;

typedef  int8_t  S1;
typedef  int16_t S2;
typedef  int32_t S4;
typedef  int64_t S8;

typedef  int8_t  B1;
typedef  int16_t B2;
typedef  int32_t B4;
typedef  int64_t B8;

typedef float    F4;
typedef double   F8;

// bits //
typedef uint8_t  U8b;
typedef uint16_t U16b;
typedef uint32_t U32b;
typedef uint64_t U64b;

typedef  int8_t  I8b;
typedef  int16_t I16b;
typedef  int32_t I32b;
typedef  int64_t I64b;

typedef  int8_t  S8b;
typedef  int16_t S16b;
typedef  int32_t S32b;
typedef  int64_t S64b;

typedef  int8_t  B8b;
typedef  int16_t B16b;
typedef  int32_t B32b;
typedef  int64_t B64b;

typedef float    F32b;
typedef double   F64b;

// named //
typedef char          Char;
typedef unsigned char Byte;
typedef int           Bool;
typedef size_t        Size;
typedef uintptr_t     UPtr;
typedef intptr_t      SPtr;
typedef void *        Addr;

#define ARR_LEN_BITS(bits) ((bits / CHAR_BIT) + (bits % CHAR_BIT > 0))

// TODO: aligned versions

// TYPE CREATION //
// NOTE: if you don't add the brackets these just act as normal: e.g. struct MyType vs struct (MyType)
#define struct(s) struct s; typedef struct s s; struct s
#define union(s)  union  s; typedef union  s s; union  s
#define enum(s)   enum   s; typedef enum   s s; enum   s

// create a label as well as a case jump point
/* #define case(c) case c: case_## c */

// CASTING //
#define cast(t, value) ((t)(value))
// "bitcast"?
#define recast(ti, to, value) (((union { ti in, char b[sizeof(value)]; to out; }){value}).out)

#endif // TYPES

#if 1 // VALUES
#if 0 // ID VALUES
// 7 char string as numeric value
// NOTE: d7 had implementation
#define STR_ID_(s, n) (((s "\0\0\0\0\0\0\0\0")[n]) << n)
#define STR_ID1(s) STR_ID_(s,0)|STR_ID_(s,1)|STR_ID_(s,2)|STR_ID_(s,3)|STR_ID_(s,4)|STR_ID_(s,5)|STR_ID_(s,6) & ~(0xFF << 7)

/* #define STR__ID(s, n) (s[n] ? s[n] : 0) << 7-n */
#define STR__ID(s, n) (((s "\0\0\0\0\0\0\0\0")[n]) << (7-n))
#define STR_ID2(s) \
    STR__ID(s,0)| \
STR__ID(s,1) |\
STR__ID(s,2) |\
STR__ID(s,3) |\
STR__ID(s,4) |\
STR__ID(s,5) |\
STR__ID(s,6) &\
~0xFF
typedef union CeeStringId {
    U8   val;
    char str[8];
} CeeStringId;
#endif // ID VALUES

#define TYPE_IS_SIGNED(t) ((t)-1 < 0)
#define VAR_IS_SIGNED(v) ((((v)^(v))-1) < 0)

// INVALID VALUES //

#define MIN_SIGNED(type) ((type)1 << (sizeof(type) * 8 - 1))
#define MAX_UNSIGNED(type) (~(type)0)

#define IS_MIN_SIGNED(v) ((v) == ((((v)^(v))+1) << (sizeof(v) * 8 - 1)) )

// for unsigned: all bits set to 1
// TODO: for signed: MIN_SIGNED
// makes valid range for signed integers symmetrical
// if only unsigned integers are needed, only loses top value
// rather than losing half of available as when `value < 0` -> invalid

#define INVALID(type) (TYPE_IS_SIGNED(type) ? MIN_SIGNED(type) : MAX_UNSIGNED(type))
// e.g. memset(new_array, INVALID(char), sizeof(new_array));
#define is_invalid(value) (VAR_IS_SIGNED(value) ? IS_MIN_SIGNED(value) : !~(value))
#define is_valid(value) (VAR_IS_SIGNED(value)   ? ! IS_MIN_SIGNED(value) : !!~(value))

// SIZES //
#ifndef KB
#define KB(x) (1024ULL *   (x))
#define MB(x) (1024ULL * KB(x))
#define GB(x) (1024ULL * MB(x))
#define TB(x) (1024ULL * GB(x))
#endif // KB
#endif // VALUES

#if 1 // SCOPES
// TODO: not sure about naming here... see how they tend to be used in practice
#define scoped(start, end) for(char cee_n = ((start), 0); !cee_n++; end)
#define before_after(when, ...) for(char const *when = "before", *cee_n = ((__VA_ARGS__), when); cee_n; when = "after", cee_n = 0, __VA_ARGS__)
// NOTE: MUST use a type
#define scope(init, end) for(init, *cee_n = 0; !cee_n; ++cee_n, (end))
#define scope_var(init)  for(init, *cee_n = 0; !cee_n; ++cee_n)
#endif // SCOPES


#if 1 // ANNOTATION
#define NOTE(x,m) x
#define yes(m) 1
#define no(m)  0
#define nonull
// ALT for nonull
// #if DEBUG
// #define not_null(p) (assert(p && "argument is null"),p)
// #endif
//
// int my_func(nonull int *nums, int count) {...}
// #define my_func(a,b) my_func(not_null(a), b)


// to be redefined at the point of use as _:  _(a, V2_add, _(b, V2_sub, c))
// #define _(a,fn,b) INFIX_PRE(V2_,a,fn,b) => _(a, add, _(b, sub, c))
//                           vs normal use => V2_add(a, V2_sub(b, c))
#define INFIX(a, fn, b)                                   fn((a), (b))
#define INFIX_PRE(pre, a, fn, b)                   pre ## fn((a), (b))
#define INFIX_POST(post, a, fn, b)                fn ## post((a), (b))
#define INFIX_PREPOST(pre, post, a, fn, b) pre ## fn ## post((a), (b))

//#define ASSUME_  typedef char assumption_failed[-1];
#define ASSUME_CHECK_0 typedef char assumption_failed[-1];
#define ASSUME_CHECK_1
#define assume(a) CEE_CAT(ASSUME_CHECK_, ASSUME_## a)
#endif // ANNOTATION


#if 1 // LOOPS
#define array_len(a) (sizeof(a)/sizeof(*(a)))
#define countof(a) (sizeof(a)/sizeof(*(a)))
/* #ifndef  offsetof */
/* # define offsetof(st, m) (&((st *)0)->m) */
/* #endif */
/* #ifndef  alignof */
/* # define alignof(t) (&((struct{char c; t m} *)0)->m) */
/* #endif */
#define ARRAY__N(a) (a), countof(a)
#define N__ARRAY(a) countof(a), (a)
#define ARRAY__SIZE(a) (a), sizeof(a)
#define SIZE__ARRAY(a) sizeof(a), (a)
#define STR_LIT__SIZE(a) (a), sizeof(a)
#define SIZE__STR_LIT(a) sizeof(a), (a)
#define STR_LIT__LEN(a) (a), (sizeof(a)-1)
#define LEN__STR_LIT(a) (sizeof(a)-1), (a)

// TODO: typeof variants
// TODO: can define struct with all components of loop...
#define each
#define all(t, a, ...) (t cee_a = __VA_ARGS__, *cee_b = cee_a, *cee_n = cee_b + countof(cee_a), a = *cee_b; cee_b != cee_n; a = *++cee_b)
// TODO: use (unsigned) -1 to allow unsigned loops to zero with same format
//#define range(t, i, start, end) (t i = (start), cee_a = ((end) < (start) ? (t)-1 : +1), cee_n = (t)(end)-((end) < (start) ? (t)1 : 0); i != cee_n; i += cee_a)
#define range(t, i, start, end) (t i = (start); i != (end); i += 1)
#define range_n(t, i, start, end_name, end_val) (t i = (start), end_name = (end_val); i != (end_val); i += 1)
#define upto(t, i, end) (t i = 0; i < end; ++i)

#define until(e) while(! (e))
#endif // LOOPS

// JUMPS //
#define case_label(x) case x: case_## x
// case_label(3): dostuff(); break;
// case_label(4): dootherstuff(); goto case_3;
#define fallthrough

// FUNCTIONS //
// XXX: meh...
#define multiple_return(types, fn) struct fn##_result types fn
/*
 * multiple_return( {int a; int b},
 * my_function_name)() {
 * 		struct my_function_name_result result = {0};
 * 		return result;
 * }
 */

// USEFUL CONSTANTS FOR LAYOUT
static const double Golden_Ratio     = 1.61803398874989484820458683436563;
static const double Phi1             = 1.61803398874989484820458683436563;
static const double Plastic_Constant = 1.32471795724474602596090885447809;
static const double Phi2             = 1.32471795724474602596090885447809;
static const double Phi3             = 1.220744084605759475361685349108831;


#define CEE_H
#endif//CEE_H