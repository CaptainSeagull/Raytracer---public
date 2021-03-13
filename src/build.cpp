#if ALLOW_ASSERTS
    #define ASSERT(expr) do { static int internal_assert_ignore = false; if(!internal_assert_ignore) {if(!(expr)) { *(uintptr_t volatile *)0 = 0; } } } while(0)
    #define LANE_ALLOW_ASSERT
    #define MEMORY_ARENA_ALLOW_ASSERT
    #define STRING_ALLOW_ASSERT
    #define TOKEN_ALLOW_ASSERT
#else
    #define ASSERT(expr) {}
#endif

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "../shared/stb_sprintf.h"

#include "../shared/defer.h"

#define STRING_IMPLEMENTATION
#define STRING_PUBLIC_DEC static
#include "../shared/string.h"
using namespace Cpp_String;

#define MEMORY_ARENA_IMPLEMENTATION
#define MEMORY_PUBLIC_DEC static
#include "../shared/memory_arena.h"
using namespace Memory_Arena;

#define TOKEN_IMPLEMENTATION
#define TOKEN_PUBLIC_DEC static
#include "../shared/token.h"
using namespace Tokenizer_Cpp;

#define LANE_PUBLIC_DEC inline
#include "../shared/lane/lane.cpp"
using namespace lane;

#include <math.h> // sinf, cosf, pow

#include <stdint.h>

#include "utils.h"
#include "platform.h"

#include "m4x4.cpp"
#include "image.cpp"
#include "raytracer.cpp"
#include "scene_builder.cpp"
#include "main.cpp"

#if USE_SDL
    #include "../shared/SDL2/SDL.h"
    #include <stdio.h>
    #include <stdlib.h>

    #include "platform_sdl2.cpp"
#else
    #if OS_WIN32
        #include <windows.h>
        #include "platform_win32.cpp"
    #else
        #error "Unknown OS"
    #endif
#endif
