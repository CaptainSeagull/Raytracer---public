LANE_PUBLIC_DEC Lane_F32 lane_f32(float a) {
    Lane_F32 r;
    r.v = a;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32(float a, float b, float c, float d, float e, float f, float g, float h) {
    Lane_F32 r;
    r.v = a;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(uint32_t a) {
    Lane_F32 r;
    r.v = (float)a;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_from_u32(Lane_U32 a) {
    Lane_F32 r;
    r.v = (float)a.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_zero(void) {
    Lane_F32 r;
    r.v = 0;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 lane_f32_floor(Lane_F32 a) {
    Lane_F32 r;
    r.v = (float)((int32_t)a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator&(Lane_U32 a, Lane_F32 b) {
    Lane_F32 r;
    a.v = (a.v) ? 0xFFFFFFFF : 0;
    uint32_t t = (*(uint32_t *)&a.v & *(uint32_t *)&b.v);;
    r.v = *(float *)&t;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator*(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = a.v * b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator/(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = a.v / b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator+(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = a.v + b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 operator-(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = a.v - b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = a.v > b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = a.v >= b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = a.v < b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = a.v <= b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = a.v == b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_F32 a, Lane_F32 b) {
    Lane_U32 r;
    r.v = a.v != b.v;

    return(r);
}

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_F32 *dst, Lane_F32 src) {
    conditional_assign(mask, (Lane_U32 *)dst, *(Lane_U32 *)&src);
}


LANE_PUBLIC_DEC Lane_F32 square_root(Lane_F32 a) {
    Lane_F32 r;
    r.v = square_root(a.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 rsquare_root(Lane_F32 a) {
    Lane_F32 r = square_root(a); // TODO: Is this correct?

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 max(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = (a.v > b.v) ? a.v : b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 min(Lane_F32 a, Lane_F32 b) {
    Lane_F32 r;
    r.v = (a.v < b.v) ? a.v : b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 gather_f32_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_F32 r;
    r.v = *(float *)((uint8_t *)ptr + indices.v * stride);

    return(r);
}
