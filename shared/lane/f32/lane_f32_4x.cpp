LANE_PUBLIC_DEC Lane_F32 lane_f32(float a) {
    Lane_F32 r;
    r.v = _mm_set1_ps(a);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32(float a, float b, float c, float d, float e, float f, float g, float h) {
    Lane_F32 r;
    r.v = _mm_setr_ps(a, b, c, d);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(uint32_t a) {
    Lane_F32 r;
    r.v = _mm_set1_ps((float)a);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(Lane_U32 a) {
    Lane_F32 r;
    r.v = _mm_cvtepi32_ps(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_zero(void) {
    Lane_F32 r;
    r.v = _mm_setzero_ps();

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_floor(Lane_F32 a) {
    Lane_F32 r;
    r.v = _mm_floor_ps(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator&(Lane_U32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm_and_ps(_mm_castsi128_ps(a.v), b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator*(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm_mul_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator/(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm_div_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator+(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm_add_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator-(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm_sub_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm_castps_si128(_mm_cmpgt_ps(a.v, b.v));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm_castps_si128(_mm_cmpge_ps(a.v, b.v));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm_castps_si128(_mm_cmplt_ps(a.v, b.v));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm_castps_si128(_mm_cmple_ps(a.v, b.v));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm_castps_si128(_mm_cmpeq_ps(a.v, b.v));

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = _mm_castps_si128(_mm_cmpneq_ps(a.v, b.v));

    return(r);
}


LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_F32 *dst, Lane_F32 src) {
    __m128 m = _mm_castsi128_ps(mask.v);
    dst->v = _mm_or_ps(_mm_andnot_ps(m, dst->v), _mm_and_ps(m, src.v));
}

LANE_PUBLIC_DEC Lane_F32 square_root(Lane_F32 a) {
    Lane_F32 r;
    r.v = _mm_sqrt_ps(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 rsquare_root(Lane_F32 a) {
    Lane_F32 r;
    r.v = _mm_rsqrt_ps(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 max(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm_max_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 min(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = _mm_min_ps(a.v, b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 gather_f32_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_F32 r;

    uint32_t *v = (uint32_t *)&indices.v;
    r.v = _mm_setr_ps(*(float *)((uint8_t *)ptr + v[0] * stride),
                      *(float *)((uint8_t *)ptr + v[1] * stride),
                      *(float *)((uint8_t *)ptr + v[2] * stride),
                      *(float *)((uint8_t *)ptr + v[3] * stride));

    return(r);
}
