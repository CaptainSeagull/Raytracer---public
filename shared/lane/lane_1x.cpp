#if defined(LANE_USE_CRT_MATH)
    #include <math.h> // sqrt
#else
    #include <intrin.h>
#endif

float square_root(float a) {
    float r;
#if defined(LANE_USE_CRT_MATH)
    r = sqrt(a);
#else
    __m128 va = _mm_set1_ps(a);
    __m128 vr = _mm_sqrt_ps(va);
    r = ((float *)&vr)[0];
#endif
    return(r);
}
