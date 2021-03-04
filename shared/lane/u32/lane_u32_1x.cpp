// Constructors
LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a) {
    Lane_U32 r;
    r.v = a;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 lane_u32(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h) {
    Lane_U32 r;
    r.v = a;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<<(Lane_U32 a, uint32_t s) {
    Lane_U32 r;
    r.v = a.v << s;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>>(Lane_U32 a, uint32_t s) {
    Lane_U32 r;
    r.v = a.v >> s;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator^(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r = a.v ^ b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator|(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v | b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator&(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v & b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator*(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v * b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator/(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v / b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator+(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v + b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v - b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a) {
    Lane_U32 r;
    r.v = 0 - a.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v > b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator>=(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v >= b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v < b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator<=(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v <= b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v == b.v;

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = a.v != b.v;
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 and_not(Lane_U32 a, Lane_U32 b) {
    Lane_U32 r;
    r.v = (~a.v & b.v);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 not_zero(Lane_U32 a) {
    Lane_U32 r;
    r.v = a.v != 0;

    return(r);
}

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_U32 *dst, Lane_U32 src) {
    uint32_t m = (mask.v) ? 0xFFFFFFFF : 0;
    dst->v = ((~m & dst->v) | (m & src.v));
}


LANE_PUBLIC_DEC int/*Bool*/ lane_mask_is_zeroed(Lane_U32 a) {
    int r = (a.v == 0);

    return(r);
}

LANE_PUBLIC_DEC int/*Bool*/ lane_mask_is_full(Lane_U32 a) {
    int r = (a.v != 0);

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!(Lane_U32 a) {
    Lane_U32 r;
    r.v = !a.v;

    return(r);
}