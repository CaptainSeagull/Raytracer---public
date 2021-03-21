#if ALLOW_ASSERTS
    #define ASSERT(expr) do { static int internal_assert_ignore = false; if(!internal_assert_ignore) {if(!(expr)) { *(uintptr_t volatile *)0 = 0; } } } while(0)

    #define LANE_ALLOW_ASSERT
    #define MEMORY_ARENA_ALLOW_ASSERT
    #define STRING_ALLOW_ASSERT
    #define TOKEN_ALLOW_ASSERT
#else
    #define ASSERT(expr) {}
#endif

#if INTERNAL
    #define MEMORY_ARENA_WRITE_ERRORS
#endif

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "../shared/stb_sprintf.h"

#define STRING_IMPLEMENTATION
#define STRING_PUBLIC_DEC static
#include "../shared/string.h"

#define MEMORY_ARENA_IMPLEMENTATION
#define MEMORY_PUBLIC_DEC static
#include "../shared/memory_arena.h"

#define TOKEN_IMPLEMENTATION
#define TOKEN_PUBLIC_DEC static
#include "../shared/token.h"

#define LANE_PUBLIC_DEC inline
#include "../shared/lane/lane.cpp"
using namespace lane;

#include <math.h> // sinf, cosf, pow

#include <stdint.h>

#if USE_OPENCL
    #include "../shared/cl/cl.h"
#endif

#include "utils.h"
#include "debug_output.cpp"

#include "platform.h"

#include "m4x4.cpp"
#include "image.cpp"
#include "raytracer.h"