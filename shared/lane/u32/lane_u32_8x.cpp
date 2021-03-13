LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a) {
    Lane_U32 r;
    r.v = _mm256_set1_epi32(a);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h) {
    Lane_U32 r;
    r.v = _mm256_set_epi32(a, b, c, d, e, f, g, h);

    return(r);
}


LANE_PUBLIC_DEC Lane_U32 lane_u32_from_f32(float a) {
    Lane_U32 r;
    r.v = _mm256_set1_epi32((uint32_t)a);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 lane_u32_from_f32(Lane_F32 a) {
    Lane_U32 r;
    r.v = _mm256_cvtps_epi32(_mm256_floor_ps(a.v));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<<(Lane_U32 a, uint32_t s) {
    Lane_U32 r;
    r.v = _mm256_slli_epi32(a.v, s);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>>(Lane_U32 a, uint32_t s) {
    Lane_U32 r;
    r.v = _mm256_srli_epi32(a.v, s);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator^(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_xor_si256(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator|(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_or_si256(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator&(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_and_si256(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator*(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    //r.v = _mm256_mul_epi32(a.v, b.v);
    LANE_ASSERT(0); // TODO: This seems to not work... investigate?

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator+(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_add_epi32(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_sub_epi32(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a) {
    Lane_U32 r;
    r.v = _mm256_sub_epi32(_mm256_setzero_si256(), a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_cmpeq_epi32(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_xor_si256(_mm256_cmpeq_epi32(a.v, b.v), _mm256_set1_epi32(0xFFFFFFFF)); // TODO: More efficient way to do this comparison?

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 and_not(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm256_andnot_si256(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 not_zero(Lane_U32 a) {
    Lane_U32 r;
    r.v = _mm256_cmpeq_epi32(a.v, _mm256_setzero_si256());

    return(r);
}

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_U32 *dst, Lane_U32 src) {
    *dst = (and_not(mask, *dst) | (mask & src));
}



LANE_PUBLIC_DEC int/*bool*/ lane_mask_is_zeroed(Lane_U32 a) {
    int v = _mm256_movemask_epi8(a.v);
    int r = (v == 0);
    return(r);
}

LANE_PUBLIC_DEC int/*bool*/ lane_mask_is_full(Lane_U32 a) {
    int r = (_mm256_movemask_epi8(a.v) == 0xFFFFFFFF);
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!(Lane_U32 a) {
    Lane_U32 r;
    __m256i b = _mm256_set1_epi32(1);
    r.v = _mm256_andnot_si256(a.v, b);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 gather_u32_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_U32 r;

    // TODO: Could I use blend in here for a gather?
    uint32_t *v = (uint32_t *)&indices.v;
    r.v = _mm256_set_epi32(*(uint32_t *)((uint8_t *)ptr + v[0] * stride),
                           *(uint32_t *)((uint8_t *)ptr + v[1] * stride),
                           *(uint32_t *)((uint8_t *)ptr + v[2] * stride),
                           *(uint32_t *)((uint8_t *)ptr + v[3] * stride),
                           *(uint32_t *)((uint8_t *)ptr + v[4] * stride),
                           *(uint32_t *)((uint8_t *)ptr + v[5] * stride),
                           *(uint32_t *)((uint8_t *)ptr + v[6] * stride),
                           *(uint32_t *)((uint8_t *)ptr + v[7] * stride));

    return(r);
}
