LANE_PUBLIC_DEC Lane_F32 lane_f32(float a) {
    Lane_F32 r;
    r.v = _mm256_set1_ps(a);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(uint32_t a) {
    Lane_F32 r;
    r.v = _mm256_set1_ps((float)a);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(Lane_U32 a) {
    Lane_F32 r;
    r.v = _mm256_cvtepi32_ps(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_zero(void) {
    Lane_F32 r;
    r.v = _mm256_setzero_ps();

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator&(Lane_U32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm256_and_ps(_mm256_castsi256_ps(a.v), b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator*(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm256_mul_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator/(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm256_div_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator+(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm256_add_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator-(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm256_sub_ps(a.v, b.v);

    return(r);
}

// TODO: For _CMP_... methods, do we want Ordered Signalling or not?

LANE_PUBLIC_DEC Lane_U32 operator>(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_GT_OQ));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_GE_OQ));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_LT_OQ));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_LE_OQ));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_EQ_OQ));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_NEQ_OQ));

    return(r);
}


LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_F32 *dst, Lane_F32 src) {
    __m256 m = _mm256_castsi256_ps(mask.v);
    dst->v = _mm256_or_ps(_mm256_andnot_ps(m, dst->v), _mm256_and_ps(m, src.v));
}

LANE_PUBLIC_DEC Lane_F32 square_root(Lane_F32 a) {
    Lane_F32 r;
    r.v = _mm256_sqrt_ps(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 rsquare_root(Lane_F32 a) {
    Lane_F32 r;
    r.v = _mm256_rsqrt_ps(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_max(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm256_max_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_min(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm256_min_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 gather_f32_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_F32 r;

    // TODO: Could I use blend in here for a gather?
    uint32_t *v = (uint32_t *)&indices.v;
    r.v = _mm256_setr_ps(*(float *)((uint8_t *)ptr + v[0] * stride),
                         *(float *)((uint8_t *)ptr + v[1] * stride),
                         *(float *)((uint8_t *)ptr + v[2] * stride),
                         *(float *)((uint8_t *)ptr + v[3] * stride),
                         *(float *)((uint8_t *)ptr + v[4] * stride),
                         *(float *)((uint8_t *)ptr + v[5] * stride),
                         *(float *)((uint8_t *)ptr + v[6] * stride),
                         *(float *)((uint8_t *)ptr + v[7] * stride));

    return(r);
}

