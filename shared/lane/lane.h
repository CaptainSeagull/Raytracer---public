// Lane v0.1

// #define LANE_ALLOW_ASSERTS => Allow allow/disable asserts.
// #define LANE_INTERNAL      => Allow some internal reporting (like asserts).
// #define LANE_ASSERT(x)     => Override default assert.

#if !defined(LANE_H)
#define LANE_H

#include <stdint.h>

#if !defined(LANE_PUBLIC_DEC)
    #define LANE_PUBLIC_DEC
#endif

// Environment
#if !defined(LANE_COMPILER_MSVC) && !defined(LANE_COMPILER_CLANG) && !defined(LANE_COMPILER_GCC)
    #define LANE_COMPILER_MSVC 0
    #define LANE_COMPILER_CLANG 0
    #define LANE_COMPILER_GCC 0

    #if defined(__clang__)
        #undef LANE_COMPILER_CLANG
        #define LANE_COMPILER_CLANG 1
    #elif defined(_MSC_VER)
        #undef LANE_COMPILER_MSVC
        #define LANE_COMPILER_MSVC 1
    #elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.
        #undef LANE_COMPILER_GCC
        #define LANE_COMPILER_GCC 1
    #else
        #error "Could not detect compiler."
    #endif
#endif

#if !defined(LANE_OS_WIN32) && !defined(LANE_OS_LINUX)
    #define LANE_OS_WIN32 0
    #define LANE_OS_LINUX 0

    #if defined(__linux__)
        #undef LANE_OS_LINUX
        #define LANE_OS_LINUX 1
    #elif defined(_WIN32)
        #undef LANE_OS_WIN32
        #define LANE_OS_WIN32 1
    #else
        #error "Could not detect OS"
    #endif
#endif

#if !defined(LANE_ENVIRONMENT64) && !defined(LANE_ENVIRONMENT32)
    #define LANE_ENVIRONMENT64 0
    #define LANE_ENVIRONMENT32 0

    #if LANE_OS_LINUX
        #if (__x86_64__ || __ppc64__)
            #undef LANE_ENVIRONMENT64
            #define LANE_ENVIRONMENT64 1
        #else
            #undef LANE_ENVIRONMENT32
            #define LANE_ENVIRONMENT32 1
        #endif
    #elif LANE_OS_WIN32
        #if defined(_WIN64)
            #undef LANE_ENVIRONMENT64
            #define LANE_ENVIRONMENT64 1
        #else
            #undef LANE_ENVIRONMENT32
            #define LANE_ENVIRONMENT32 1
        #endif
    #endif
#endif

// The max supported LANE_WIDTH
#define MAX_LANE_WIDTH 8

// TODO: Horizontal sub/mul would be cool. Horizontal min/max too.

// Lane width
#if (LANE_WIDTH!=1)
    #if LANE_COMPILER_MSVC
        #include <intrin.h>
    #elif (LANE_COMPILER_CLANG || LANE_COMPILER_GCC)
        #include <x86intrin.h>
    #endif
#endif

#if (LANE_WIDTH==1)
    typedef uint32_t Lane_U32_Internal;
    typedef float    Lane_F32_Internal;
#elif (LANE_WIDTH==4)
    typedef __m128i Lane_U32_Internal;
    typedef __m128  Lane_F32_Internal;
#elif (LANE_WIDTH==8)
    typedef __m256i Lane_U32_Internal;
    typedef __m256  Lane_F32_Internal;
#else
    #error "Unsupported LANE_WIDTH"
#endif

//
// Macro utils
#define LANE_FLT_MAX 3.402823466e+38F // TODO: MSVC specific?
#define LANE_FLT_MIN -LANE_FLT_MAX
#define LANE_PI32 3.14159265359f
#define LANE_TAU32 6.28318530717958647692f

#define LANE_U32_MAX ((uint32_t)-1)
#define LANE_U64_MAX ((uint64_t)-1)

#define LANE_DEGREES_TO_RADIANS(n) (((n)) * 0.0174533f)
#define LANE_RADIANS_TO_DEGREES(n) (((n)) / 0.0174533f)

namespace lane {

//
// Forward declaration of all structs.
struct Lane_F32;
struct Lane_U32;

union V2;
union V3;
union V4;

union Lane_V2;
union Lane_V3;
union Lane_V4;

//
// Methods
LANE_PUBLIC_DEC float max(float a, float b);
LANE_PUBLIC_DEC float min(float a, float b);
LANE_PUBLIC_DEC float clamp(float a, float l, float u);
LANE_PUBLIC_DEC float clamp01(float a);
LANE_PUBLIC_DEC float lerp(float t, float a, float b);

//
// Entropy / random numbers
LANE_PUBLIC_DEC float random_unilateral(uint32_t *entropy_series);
LANE_PUBLIC_DEC float random_bilateral(uint32_t *entropy_series);
LANE_PUBLIC_DEC uint32_t xorshift32(uint32_t *entropy_series);

LANE_PUBLIC_DEC Lane_F32 random_unilateral(Lane_U32 *entropy_series);
LANE_PUBLIC_DEC Lane_F32 random_bilateral(Lane_U32 *entropy_series);
LANE_PUBLIC_DEC Lane_U32 xorshift32(Lane_U32 *entropy_series);

//
// V2
union V2 {
    struct { float x, y; };
    struct { float w, h; };

    float e[2];
};
static_assert(sizeof(V2) == sizeof(float) * 2, "V2 size wrong");

LANE_PUBLIC_DEC V2 v2(V2 a);
LANE_PUBLIC_DEC V2 v2(float x, float y);
LANE_PUBLIC_DEC V2 v2(float a);

LANE_PUBLIC_DEC V2 operator*(V2 a, float b);
LANE_PUBLIC_DEC V2 operator*(float a, V2 b);
LANE_PUBLIC_DEC V2 operator*=(V2 &a, float b);

LANE_PUBLIC_DEC V2 operator/(float a, V2 b);
LANE_PUBLIC_DEC V2 operator/(V2 a, float b);
LANE_PUBLIC_DEC V2 operator/=(V2 &a, float b);

LANE_PUBLIC_DEC V2 operator+(V2 a, V2 b);
LANE_PUBLIC_DEC V2 operator+(V2 a, V2 b);
LANE_PUBLIC_DEC V2 operator+(V2 a, V2 b);
LANE_PUBLIC_DEC V2 operator+=(V2 &a, V2 b);

LANE_PUBLIC_DEC V2 operator-(V2 a, V2 b);
LANE_PUBLIC_DEC V2 operator-(V2 a, V2 b);
LANE_PUBLIC_DEC V2 operator-(V2 a, V2 b);
LANE_PUBLIC_DEC V2 operator-(V2 a);
LANE_PUBLIC_DEC V2 operator-=(V2 &a, V2 b);

LANE_PUBLIC_DEC V2 operator&(uint32_t a, V2 b);

LANE_PUBLIC_DEC V2 normalise_or_zero(V2 a);
LANE_PUBLIC_DEC float inner(V2 a, V2 b);
LANE_PUBLIC_DEC V2 hadamard(V2 a, V2 b);
LANE_PUBLIC_DEC float length_sq(V2 a);
LANE_PUBLIC_DEC float length(V2 a);
LANE_PUBLIC_DEC V2 normalise(V2 a);
LANE_PUBLIC_DEC V2 lerp(float t, V2 a, V2 b);

//
// V3
union V3 {
    struct { float x, y, z;   };
    struct { V2 xy; float _0; };
    struct { float _1; V2 yz; };

    struct { float r, g, b;   };
    struct { V2 rg; float _2; };
    struct { float _3; V2 gb; };

    float e[3];
};
static_assert(sizeof(V3) == sizeof(float) * 3, "V3 size wrong");

LANE_PUBLIC_DEC V3 v3(V3 a);
LANE_PUBLIC_DEC V3 v3(float x, float y, float z);
LANE_PUBLIC_DEC V3 v3(float a);

LANE_PUBLIC_DEC V3 operator*(V3 a, float b);
LANE_PUBLIC_DEC V3 operator*(float a, V3 b);
LANE_PUBLIC_DEC V3 operator*=(V3 &a, float b);

LANE_PUBLIC_DEC V3 operator/(V3 a, float b);
LANE_PUBLIC_DEC V3 operator/=(V3 &a, float b);

LANE_PUBLIC_DEC V3 operator+(V3 a, V3 b);
LANE_PUBLIC_DEC V3 operator+(V3 a, V3 b);
LANE_PUBLIC_DEC V3 operator+(V3 a, V3 b);
LANE_PUBLIC_DEC V3 operator+=(V3 &a, V3 b);

LANE_PUBLIC_DEC V3 operator-(V3 a, V3 b);
LANE_PUBLIC_DEC V3 operator-(V3 a, V3 b);
LANE_PUBLIC_DEC V3 operator-(V3 a, V3 b);
LANE_PUBLIC_DEC V3 operator-(V3 a);
LANE_PUBLIC_DEC V3 operator-=(V3 &a, V3 b);

LANE_PUBLIC_DEC V3 operator&(uint32_t a, V3 b);

LANE_PUBLIC_DEC float inner(V3 a, V3 b);
LANE_PUBLIC_DEC V3 hadamard(V3 a, V3 b);
LANE_PUBLIC_DEC V3 cross(V3 a, V3 b);
LANE_PUBLIC_DEC float length_sq(V3 a);
LANE_PUBLIC_DEC float length(V3 a);
LANE_PUBLIC_DEC V3 normalise(V3 a);
LANE_PUBLIC_DEC V3 normalise_or_zero(V3 a);

//
// V4
union V4 {
    struct { float x, y, z, w;          };
    struct { V2 xy, zw;                 };
    struct { float _0; V2 yz; float _1; };
    struct { V3 xyz; float _2;          };
    struct { float _3; V3 yzw;          };

    struct { float r, g, b, a;          };
    struct { V2 rg, ba;                 };
    struct { float _4; V2 gb; float _5; };
    struct { V3 rgb; float _6;          };
    struct { float _7; V3 gba;          };

    float e[4];
};
static_assert(sizeof(V4) == sizeof(float) * 4, "V4 size wrong");

LANE_PUBLIC_DEC V4 v4(V4 a);
LANE_PUBLIC_DEC V4 v4(float x, float y, float z, float w);
LANE_PUBLIC_DEC V4 v4(float a);

LANE_PUBLIC_DEC V4 operator*(V4 a, float b);
LANE_PUBLIC_DEC V4 operator*(float a, V4 b);
LANE_PUBLIC_DEC V4 operator*=(V4 &a, float b);

LANE_PUBLIC_DEC V4 operator/(float a, V4 b);
LANE_PUBLIC_DEC V4 operator/(V4 a, float b);
LANE_PUBLIC_DEC V4 operator/=(V4 &a, float b);

LANE_PUBLIC_DEC V4 operator+(V4 a, V4 b);
LANE_PUBLIC_DEC V4 operator+(V4 a, V4 b);
LANE_PUBLIC_DEC V4 operator+(V4 a, V4 b);
LANE_PUBLIC_DEC V4 operator+=(V4 &a, V4 b);

LANE_PUBLIC_DEC V4 operator-(V4 a, V4 b);
LANE_PUBLIC_DEC V4 operator-(V4 a, V4 b);
LANE_PUBLIC_DEC V4 operator-(V4 a, V4 b);
LANE_PUBLIC_DEC V4 operator-(V4 a);
LANE_PUBLIC_DEC V4 operator-=(V4 &a, V4 b);

LANE_PUBLIC_DEC V4 operator&(uint32_t a, V4 b);

LANE_PUBLIC_DEC V4 normalise_or_zero(V4 a);
LANE_PUBLIC_DEC float inner(V4 a, V4 b);
LANE_PUBLIC_DEC V4 hadamard(V4 a, V4 b);
LANE_PUBLIC_DEC float length_sq(V4 a);
LANE_PUBLIC_DEC float length(V4 a);
LANE_PUBLIC_DEC V4 normalise(V4 a);

//
// Lane_F32
struct Lane_F32 {
    Lane_F32_Internal v;

    Lane_F32 operator=(float a);
};

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_F32 *dst, Lane_F32 src);
LANE_PUBLIC_DEC Lane_F32 lane_f32(float a);
LANE_PUBLIC_DEC Lane_F32 lane_f32(float a, float b, float c, float d, float e, float f, float g, float h);
LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(uint32_t a);
LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(Lane_U32 a);
LANE_PUBLIC_DEC Lane_F32 lane_f32_zero(void);
LANE_PUBLIC_DEC Lane_F32 lane_f32_floor(Lane_F32 a);
LANE_PUBLIC_DEC float extract(Lane_F32 a, uint32_t lane);

LANE_PUBLIC_DEC Lane_F32 operator&(Lane_U32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator*(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator*(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_F32 operator*(float a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator*=(Lane_F32 &a, Lane_F32 b);

LANE_PUBLIC_DEC Lane_F32 operator/(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator/(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_F32 operator/(float a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator/=(Lane_F32 &a, Lane_F32 b);

LANE_PUBLIC_DEC Lane_F32 operator+(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator+(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_F32 operator+(float a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator+=(Lane_F32 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator++(Lane_F32 &a);

LANE_PUBLIC_DEC Lane_F32 operator-(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator-(Lane_F32 a);
LANE_PUBLIC_DEC Lane_F32 operator-(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_F32 operator-(float a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator-=(Lane_F32 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 operator--(Lane_F32 &a);

LANE_PUBLIC_DEC Lane_U32 operator>(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_U32 operator>=(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_U32 operator<(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_U32 operator<=(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_U32 operator>(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_U32 operator>=(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_U32 operator<(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_U32 operator<=(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_F32 a, float b);
LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_F32 a, float b);

LANE_PUBLIC_DEC Lane_F32 square_root(Lane_F32 a);
LANE_PUBLIC_DEC float square_root(float a);
LANE_PUBLIC_DEC Lane_F32 rsquare_root(Lane_F32 a);
LANE_PUBLIC_DEC float rsquare_root(float a);
LANE_PUBLIC_DEC Lane_F32 max(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 max(float a,    Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 max(Lane_F32 a, float    b);
LANE_PUBLIC_DEC Lane_F32 min(Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 min(float    a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 min(Lane_F32 a, float    b);
LANE_PUBLIC_DEC Lane_F32 clamp(Lane_F32 a, Lane_F32 l, Lane_F32 u);
LANE_PUBLIC_DEC Lane_F32 clamp(Lane_F32 a, float l, float u);
LANE_PUBLIC_DEC Lane_F32 clamp01(Lane_F32 a);
LANE_PUBLIC_DEC Lane_F32 lerp(Lane_F32 t, Lane_F32 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_F32 lerp(float t, Lane_F32 a, Lane_F32 b);

LANE_PUBLIC_DEC float horizontal_add(Lane_F32 a);
#define GATHER_F32(ptr, i, mem) gather_f32_internal(&(ptr)->mem, sizeof(*(ptr)), i)
LANE_PUBLIC_DEC Lane_F32 gather_f32_internal(void *ptr, uint64_t stride, Lane_U32 indices);

//
// Lane_U32
// NOTE: This should only really be used as a boolean flag... VERY underdeveloped!
struct Lane_U32 {
    Lane_U32_Internal v;

    Lane_U32 operator=(uint32_t a);
};

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_U32 *dst, Lane_U32 src);
LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a);
LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h);
LANE_PUBLIC_DEC Lane_U32 lane_u32_from_f32(float a);
LANE_PUBLIC_DEC Lane_U32 lane_u32_from_f32(Lane_F32 a);
LANE_PUBLIC_DEC Lane_U32 and_not(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 not_zero(Lane_U32 a);
LANE_PUBLIC_DEC uint32_t extract(Lane_U32 a, uint32_t lane);
LANE_PUBLIC_DEC int/*Bool*/ lane_mask_is_zeroed(Lane_U32 a);
LANE_PUBLIC_DEC int/*Bool*/ lane_mask_is_full(Lane_U32 a);
LANE_PUBLIC_DEC uint64_t horizontal_add(Lane_U32 a);

LANE_PUBLIC_DEC Lane_U32 operator<<(Lane_U32 a, uint32_t s);
LANE_PUBLIC_DEC Lane_U32 operator>>(Lane_U32 a, uint32_t s);
LANE_PUBLIC_DEC Lane_U32 operator^(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator^=(Lane_U32 &a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator|(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator|=(Lane_U32 &a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator&(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator&=(Lane_U32 &a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator!(Lane_U32 a);

LANE_PUBLIC_DEC Lane_U32 operator*(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator*(Lane_U32 a, uint32_t b);
LANE_PUBLIC_DEC Lane_U32 operator*(uint32_t a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator*=(Lane_U32 &a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator+(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator+(Lane_U32 a, uint32_t b);
LANE_PUBLIC_DEC Lane_U32 operator+(uint32_t a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator+=(Lane_U32 &a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a);
LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a, uint32_t b);
LANE_PUBLIC_DEC Lane_U32 operator-(uint32_t a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator-=(Lane_U32 &a, Lane_U32 b);

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_U32 a, Lane_U32 b);
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_U32 a, uint32_t b);
LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_U32 a, uint32_t b);

#define GATHER_U32(ptr, i, mem) gather_u32_internal(&(ptr)->mem, sizeof(*(ptr)), i)
LANE_PUBLIC_DEC Lane_U32 gather_u32_internal(void *ptr, uint64_t stride, Lane_U32 indices);

//
// Lane_V2
union Lane_V2 {
    struct { Lane_F32 x, y; };
    struct { Lane_F32 w, h; };

    Lane_V2 operator=(V2 a);
};
static_assert(sizeof(Lane_V2) == sizeof(float) * LANE_WIDTH * 2, "Invalid Lane_V2 size");

LANE_PUBLIC_DEC Lane_V2 lane_v2(V2 a);
LANE_PUBLIC_DEC Lane_V2 lane_v2(Lane_F32 x, Lane_F32 y);
LANE_PUBLIC_DEC Lane_V2 lane_v2(float x, float y);
LANE_PUBLIC_DEC Lane_V2 lane_v2(Lane_F32 a);
LANE_PUBLIC_DEC Lane_V2 lane_v2(float a);
LANE_PUBLIC_DEC Lane_V2 lane_v2_zero(void);

LANE_PUBLIC_DEC Lane_V2 operator*(Lane_V2 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V2 operator*(Lane_F32 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 operator*=(Lane_V2 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V2 operator*(Lane_V2 a, float b);
LANE_PUBLIC_DEC Lane_V2 operator*(float a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 operator*=(Lane_V2 &a, float b);

LANE_PUBLIC_DEC Lane_V2 operator/(Lane_V2 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V2 operator/=(Lane_V2 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V2 operator/(Lane_V2 a, float b);
LANE_PUBLIC_DEC Lane_V2 operator/=(Lane_V2 &a, float b);

LANE_PUBLIC_DEC Lane_V2 operator+(Lane_V2 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 operator+(Lane_V2 a, V2 b);
LANE_PUBLIC_DEC Lane_V2 operator+(V2 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 operator+=(Lane_V2 &a, Lane_V2 b);

LANE_PUBLIC_DEC Lane_V2 operator-(Lane_V2 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 operator-(Lane_V2 a, V2 b);
LANE_PUBLIC_DEC Lane_V2 operator-(V2 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 operator-(Lane_V2 a);
LANE_PUBLIC_DEC Lane_V2 operator-=(Lane_V2 &a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_V2 a, Lane_V2 b);

LANE_PUBLIC_DEC Lane_V2 operator&(Lane_U32 a, Lane_V2 b);

LANE_PUBLIC_DEC Lane_V2 normalise_or_zero(Lane_V2 a);
LANE_PUBLIC_DEC Lane_F32 inner(Lane_V2 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 hadamard(Lane_V2 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_F32 length_sq(Lane_V2 a);
LANE_PUBLIC_DEC Lane_F32 length(Lane_V2 a);
LANE_PUBLIC_DEC Lane_V2 normalise(Lane_V2 a);
LANE_PUBLIC_DEC Lane_V2 lerp(Lane_F32 t, Lane_V2 a, Lane_V2 b);
LANE_PUBLIC_DEC Lane_V2 lerp(float t, Lane_V2 a, Lane_V2 b);

LANE_PUBLIC_DEC V2 extract(Lane_V2 a, uint32_t lane);
LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_V2 *dst, Lane_V2 src);
LANE_PUBLIC_DEC V2 horizontal_add(Lane_V2 a);
LANE_PUBLIC_DEC Lane_V2 gather_v2_internal(void *ptr, uint64_t stride, Lane_U32 indices);

//
// Lane_V3
// TODO: Doing Lane_V3 v = {}; doesn't work anymore :-( ... can I make this work?
union Lane_V3 {
    struct { Lane_F32 x, y, z;        };
    struct { Lane_V2 xy; Lane_F32 _0; };
    struct { Lane_F32 _1; Lane_V2 yz; };

    struct { Lane_F32 r, g, b;        };
    struct { Lane_V2 rg; Lane_F32 _2; };
    struct { Lane_F32 _3; Lane_V2 gb; };

    Lane_V3 operator=(V3 a);
};
static_assert(sizeof(Lane_V3) == sizeof(float) * LANE_WIDTH * 3, "Invalid Lane_V3 size");

LANE_PUBLIC_DEC Lane_V3 lane_v3(V3 a);
LANE_PUBLIC_DEC Lane_V3 lane_v3(Lane_F32 x, Lane_F32 y, Lane_F32 z);
LANE_PUBLIC_DEC Lane_V3 lane_v3(float x, float y, float z);
LANE_PUBLIC_DEC Lane_V3 lane_v3(Lane_F32 a);
LANE_PUBLIC_DEC Lane_V3 lane_v3(float a);
LANE_PUBLIC_DEC Lane_V3 lane_v3_zero(void);

LANE_PUBLIC_DEC Lane_V3 operator*(Lane_V3 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V3 operator*(Lane_F32 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 operator*=(Lane_V3 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V3 operator*(Lane_V3 a, float b);
LANE_PUBLIC_DEC Lane_V3 operator*(float a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 operator*=(Lane_V3 &a, float b);

LANE_PUBLIC_DEC Lane_V3 operator/(Lane_V3 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V3 operator/=(Lane_V3 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V3 operator/(Lane_V3 a, float b);
LANE_PUBLIC_DEC Lane_V3 operator/=(Lane_V3 &a, float b);

LANE_PUBLIC_DEC Lane_V3 operator+(Lane_V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 operator+(Lane_V3 a, V3 b);
LANE_PUBLIC_DEC Lane_V3 operator+(V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 operator+=(Lane_V3 &a, Lane_V3 b);

LANE_PUBLIC_DEC Lane_V3 operator-(Lane_V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 operator-(Lane_V3 a, V3 b);
LANE_PUBLIC_DEC Lane_V3 operator-(V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 operator-(Lane_V3 a);
LANE_PUBLIC_DEC Lane_V3 operator-=(Lane_V3 &a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_V3 a, Lane_V3 b);

LANE_PUBLIC_DEC Lane_V3 operator&(Lane_U32 a, Lane_V3 b);

LANE_PUBLIC_DEC Lane_F32 inner(Lane_V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 hadamard(Lane_V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 cross(Lane_V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_F32 length_sq(Lane_V3 a);
LANE_PUBLIC_DEC Lane_F32 length(Lane_V3 a);
LANE_PUBLIC_DEC Lane_V3 normalise(Lane_V3 a);
LANE_PUBLIC_DEC Lane_V3 normalise_or_zero(Lane_V3 a);
LANE_PUBLIC_DEC Lane_V3 lerp(Lane_F32 t, Lane_V3 a, Lane_V3 b);
LANE_PUBLIC_DEC Lane_V3 lerp(float t, Lane_V3 a, Lane_V3 b);

LANE_PUBLIC_DEC V3 extract(Lane_V3 a, uint32_t lane);
LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_V3 *dst, Lane_V3 src);
LANE_PUBLIC_DEC V3 horizontal_add(Lane_V3 a);
#define GATHER_V3(ptr, i, mem) gather_v3_internal(&(ptr)->mem, sizeof(*(ptr)), i)
LANE_PUBLIC_DEC Lane_V3 gather_v3_internal(void *ptr, uint64_t stride, Lane_U32 indices);

//
// Lane_V4
union Lane_V4 {
    struct { Lane_F32 x, y, z, w;      };
    struct { Lane_V2 xy, zw;           };
    struct { Lane_V3 xyz; Lane_F32 _0; };
    struct { Lane_F32 _1; Lane_V3 yzw; };

    struct { Lane_F32 r, g, b, a;      };
    struct { Lane_V2 rg, ba;           };
    struct { Lane_V3 rgb; Lane_F32 _2; };
    struct { Lane_F32 _3; Lane_V3 gba; };

    Lane_V4 operator=(V4 a);
};
static_assert(sizeof(Lane_V4) == sizeof(float) * LANE_WIDTH * 4, "Invalid Lane_V4 size");

LANE_PUBLIC_DEC Lane_V4 lane_v4(V4 a);
LANE_PUBLIC_DEC Lane_V4 lane_v4(Lane_F32 x, Lane_F32 y, Lane_F32 z, Lane_F32 w);
LANE_PUBLIC_DEC Lane_V4 lane_v4(float x, float y, float z, float w);
LANE_PUBLIC_DEC Lane_V4 lane_v4(float a);
LANE_PUBLIC_DEC Lane_V4 lane_v4_zero(void);

LANE_PUBLIC_DEC Lane_V4 operator*(Lane_V4 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V4 operator*(Lane_F32 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 operator*=(Lane_V4 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V4 operator*(Lane_V4 a, float b);
LANE_PUBLIC_DEC Lane_V4 operator*(float a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 operator*=(Lane_V4 &a, float b);

LANE_PUBLIC_DEC Lane_V4 operator/(Lane_V4 a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V4 operator/=(Lane_V4 &a, Lane_F32 b);
LANE_PUBLIC_DEC Lane_V4 operator/(Lane_V4 a, float b);
LANE_PUBLIC_DEC Lane_V4 operator/=(Lane_V4 &a, float b);

LANE_PUBLIC_DEC Lane_V4 operator+(Lane_V4 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 operator+(Lane_V4 a, V4 b);
LANE_PUBLIC_DEC Lane_V4 operator+(V4 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 operator+=(Lane_V4 &a, Lane_V4 b);

LANE_PUBLIC_DEC Lane_V4 operator-(Lane_V4 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 operator-(Lane_V4 a, V4 b);
LANE_PUBLIC_DEC Lane_V4 operator-(V4 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 operator-(Lane_V4 a);
LANE_PUBLIC_DEC Lane_V4 operator-=(Lane_V4 &a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_V4 a, Lane_V4 b);

LANE_PUBLIC_DEC Lane_V4 operator&(Lane_U32 a, Lane_V4 b);

LANE_PUBLIC_DEC Lane_V4 normalise_or_zero(Lane_V4 a);
LANE_PUBLIC_DEC Lane_F32 inner(Lane_V4 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 hadamard(Lane_V4 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_F32 length_sq(Lane_V4 a);
LANE_PUBLIC_DEC Lane_F32 length(Lane_V4 a);
LANE_PUBLIC_DEC Lane_V4 normalise(Lane_V4 a);
LANE_PUBLIC_DEC Lane_V4 lerp(Lane_F32 t, Lane_V4 a, Lane_V4 b);
LANE_PUBLIC_DEC Lane_V4 lerp(float t, Lane_V4 a, Lane_V4 b);

LANE_PUBLIC_DEC V4 extract(Lane_V4 a, uint32_t lane);
LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_V4 *dst, Lane_V4 src);
LANE_PUBLIC_DEC V4 horizontal_add(Lane_V4 a);
#define GATHER_V4(ptr, i, mem) gather_v4_internal(&(ptr)->mem, sizeof(*(ptr)), i)
LANE_PUBLIC_DEC Lane_V4 gather_v4_internal(void *ptr, uint64_t stride, Lane_U32 indices);

// TODO: Matrix stuff!

// Mat2x2
// TODO: Could this just be a typedef for V4?
struct Lane_M2x2 {
    Lane_F32 a, b, c, d;
};

LANE_PUBLIC_DEC Lane_M2x2 lane_outer(V2 a, V2 b);

} // namespace lane

#endif


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2021 Jonathan Livingstone
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
