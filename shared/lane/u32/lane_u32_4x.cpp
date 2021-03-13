LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a) {
    Lane_U32 r;
    r.v = _mm_set1_epi32(a);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h) {
    Lane_U32 r;
    r.v = _mm_set_epi32(a, b, c, d);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 lane_u32_from_f32(float a) {
    Lane_U32 r;
    r.v = _mm_set1_epi32((uint32_t)a);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 lane_u32_from_f32(Lane_F32 a) {
    Lane_U32 r;
    r.v = _mm_cvtps_epi32(_mm_floor_ps(a.v));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<<(Lane_U32 a, uint32_t s) {
    Lane_U32 r;
    r.v = _mm_slli_epi32(a.v, s);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>>(Lane_U32 a, uint32_t s) {
    Lane_U32 r;
    r.v = _mm_srli_epi32(a.v, s);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator^(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_xor_si128(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator|(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_or_si128(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator&(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_and_si128(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator*(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_mul_epu32(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator+(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_add_epi32(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_sub_epi32(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a) {
    Lane_U32 r;
    r.v = _mm_sub_epi32(_mm_setzero_si128(), a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_cmpeq_epi32(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_xor_si128(_mm_cmpeq_epi32(a.v, b.v), _mm_set1_epi32(0xFFFFFFFF)); // TODO: More efficient way to do this comparison?

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 and_not(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = _mm_andnot_si128(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 not_zero(Lane_U32 a) {
    Lane_U32 r;
    r.v = _mm_cmpeq_epi32(a.v, _mm_setzero_si128());

    return(r);
}

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_U32 *dst, Lane_U32 src) {
    *dst = (and_not(mask, *dst) | (mask & src));
}


LANE_PUBLIC_DEC int/*Bool*/ lane_mask_is_zeroed(Lane_U32 a) {
    int r = (_mm_movemask_epi8(a.v) == 0);
    return(r);
}

LANE_PUBLIC_DEC int/*Bool*/ lane_mask_is_full(Lane_U32 a) {
    int r = (_mm_movemask_epi8(a.v) == 0xFFFFFFFF);
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!(Lane_U32 a) {
    Lane_U32 r;
    __m128i b = _mm_set1_epi32(1);
    r.v = _mm_andnot_si128(a.v, b);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 gather_u32_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_U32 r;

    // TODO: Could I use blend in here for a gather?
    uint32_t *v = (uint32_t *)&indices.v;
    r.v = _mm_set_epi32(*(uint32_t *)((uint8_t *)ptr + v[0] * stride),
                        *(uint32_t *)((uint8_t *)ptr + v[1] * stride),
                        *(uint32_t *)((uint8_t *)ptr + v[2] * stride),
                        *(uint32_t *)((uint8_t *)ptr + v[3] * stride));

    return(r);
}
