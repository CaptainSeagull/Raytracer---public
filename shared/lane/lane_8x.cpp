
float square_root(float a) {
    __m256 va = _mm256_set1_ps(a);
    __m256 vr = _mm256_sqrt_ps(va);

    float r = ((float *)&vr)[0];
    return(r);
}