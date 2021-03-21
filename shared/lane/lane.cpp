#if !defined(LANE_CPP)
#define LANE_CPP

#include "lane.h"

// Use SSE2 sqrt instead of math.h unless LANE_USE_CRT_MATH is defined
#if LANE_WIDTH==1
    #if defined(LANE_USE_CRT_MATH)
        #include <math.h> // sqrt
    #else
        #if LANE_COMPILER_MSVC
            #include <intrin.h>
        #elif (LANE_COMPILER_CLANG || LANE_COMPILER_GCC)
            #include <x86intrin.h>
        #endif
    #endif
#endif

namespace lane {

#if !defined(LANE_ALLOW_ASSERTS)
    #define LANE_ASSERT(exp) do { static int ignore = 0; if(!ignore) { if(!(exp)) {*(uint64_t volatile *)0 = 0; } } } while(0)
#else
    #define LANE_ASSERT(exp) {}
#endif

#include "lane_common.cpp"

#include "u32/lane_u32_common.cpp"
#include "f32/lane_f32_common.cpp"

#include "v2/lane_v2_common.cpp"
#include "v3/lane_v3_common.cpp"
#include "v4/lane_v4_common.cpp"

#include "v2/v2.cpp"
#include "v3/v3.cpp"
#include "v4/v4.cpp"

#if (LANE_WIDTH==1)
    #include "lane_1x.cpp"
    #include "u32/lane_u32_1x.cpp"
    #include "f32/lane_f32_1x.cpp"
#elif (LANE_WIDTH==4)
    #include "lane_4x.cpp"
    #include "u32/lane_u32_4x.cpp"
    #include "f32/lane_f32_4x.cpp"
#elif (LANE_WIDTH==8)
    #include "lane_8x.cpp"
    #include "u32/lane_u32_8x.cpp"
    #include "f32/lane_f32_8x.cpp"
#else
    #error "Unsupported LANE_WIDTH"
#endif

} // namespace lane

#endif // !defined(LANE_CPP)