typedef uint64_t U64;
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t U8;

typedef int64_t S64;
typedef int32_t S32;
typedef int16_t S16;
typedef int8_t S8;

typedef S32 Int;
typedef U32 Uint;

typedef S32 Bool;
typedef void Void;
typedef char Char;

typedef U8 Byte;
typedef uintptr_t Uintptr;
typedef intptr_t Intptr;

typedef float F32;
typedef double F64;
typedef F32 Float;

#define U32_MAX ((U32)-1)
#define U64_MAX ((U64)-1)

#define ARRAY_COUNT(arr) (sizeof(arr) / (sizeof(*(arr))))
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

//
// Detect compiler/platform.
//
#define COMPILER_MSVC 0
#define COMPILER_CLANG 0
#define COMPILER_GCC 0

#define ENVIRONMENT64 0
#define ENVIRONMENT32 0

#define OS_WIN32 0
#define OS_LINUX 0

#if defined(__clang__)
    #undef COMPILER_CLANG
    #define COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #undef COMPILER_MSVC
    #define COMPILER_MSVC 1
#elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.
    #undef COMPILER_GCC
    #define COMPILER_GCC 1
#else
    #error "Could not detect compiler."
#endif

#if defined(__linux__)
    #undef OS_LINUX
    #define OS_LINUX 1
#elif defined(_WIN32)
    #undef OS_WIN32
    #define OS_WIN32 1
#else
    #error "Could not detect OS"
#endif

#if OS_LINUX
    #if (__x86_64__ || __ppc64__)
        #undef ENVIRONMENT64
        #define ENVIRONMENT64 1
    #else
        #undef ENVIRONMENT32
        #define ENVIRONMENT32 1
    #endif
#elif OS_WIN32
    #if defined(_WIN64)
        #undef ENVIRONMENT64
        #define ENVIRONMENT64 1
    #else
        #undef ENVIRONMENT32
        #define ENVIRONMENT32 1
    #endif
#endif

#define ALIGN_POW2(v, align) (((v) + ((align) - 1)) & ~((align) - 1))
#define ALIGN4(v)            ALIGN_POW2((v), 4)
#define ALIGN8(v)            ALIGN_POW2((v), 8)
#define ALIGN16(v)           ALIGN_POW2((v), 16)
#define ALIGN32(v)           ALIGN_POW2((v), 32)

#define BYTES(v)     ((v)            * (8LL))
#define KILOBYTES(v) ((v)            * (1024LL))
#define MEGABYTES(v) ((KILOBYTES(v)) * (1024LL))
#define GIGABYTES(v) ((MEGABYTES(v)) * (1024LL))

#define internal static
#define internal_global static

enum Memory_Index {
    Memory_Index_permanent,
    Memory_Index_temp,
    Memory_Index_world_objects,
    Memory_Index_json_parse,
    Memory_Index_bitmap,
    Memory_Index_count
};

internal U32 safe_truncate_size_64(U64 v) {
    ASSERT(v <= 0xFFFFFFFF);
    U32 res = (U32)v;

    return(res);
}

internal Void *memset(Void *dst, U8 x, Uintptr size) {
    U8 *dst8 = (U8 *)dst;
    while(size--) {
        *dst8++ = x;
    }
    return(dst);
}

internal Void *zero(Void *dst, Uintptr size) {
    U8 *dst8 = (U8 *)dst;
    while(size--) {
        *dst8++ = 0;
    }

    return(dst);
}

internal Void *memcpy(Void *dst, Void *src, Uintptr count) {
    U8 *dst8 = (U8 *)dst;
    U8 *src8 = (U8 *)src;
    while(count--) {
        *dst8++ = *src8++;
    }

    return(dst);
}

internal F32 linear_to_srgb(F32 u) {
    // From: https://en.wikipedia.org/wiki/SRGB#Specification_of_the_transformation
    u = clamp(u, 0.0f, 1.0f);

    F32 s = 12.92f * u;
    if(u > 0.0031308f) {
        s = 1.055f * pow(u, 1.0f / 2.4f) - 0.055f;
    }

    return(s);
}

internal V4 linear_to_srgb(V3 u) {
    V4 r;
    r.r = 255.0f * linear_to_srgb(u.r);
    r.g = 255.0f * linear_to_srgb(u.g);
    r.b = 255.0f * linear_to_srgb(u.b);
    r.a = 255.0f;

    return(r);
}

internal U32 round_f32_to_u32(F32 f) {
    U32 res = (U32)(f + 0.5f);
    return(res);
}
