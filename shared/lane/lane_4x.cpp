
float square_root(float a) {
    __m128 va = _mm_set1_ps(a);
    __m128 vr = _mm_sqrt_ps(va);

    float r = ((float *)&vr)[0];
    return(r);
}
