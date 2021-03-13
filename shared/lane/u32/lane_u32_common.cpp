Lane_U32 Lane_U32::operator=(uint32_t a) {
    *this = lane_u32(a);
    return(*this);
}

LANE_PUBLIC_DEC Lane_U32 operator^=(Lane_U32 &a, Lane_U32 b) {
    a = a ^ b;
    return(a);
}

LANE_PUBLIC_DEC Lane_U32 operator|=(Lane_U32 &a, Lane_U32 b) {
    a = a | b;
    return(a);
}

LANE_PUBLIC_DEC Lane_U32 operator&=(Lane_U32 &a, Lane_U32 b) {
    a = a & b;
    return(a);
}

LANE_PUBLIC_DEC Lane_U32 operator*(Lane_U32 a, uint32_t b) {
    Lane_U32 r = a * lane_u32(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator*(uint32_t a, Lane_U32 b) {
    Lane_U32 r = lane_u32(a) * b;
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator*=(Lane_U32 &a, Lane_U32 b) {
    a = a * b;
    return(a);
}

LANE_PUBLIC_DEC Lane_U32 operator+(Lane_U32 a, uint32_t b) {
    Lane_U32 r = a + lane_u32(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator+(uint32_t a, Lane_U32 b) {
    Lane_U32 r = lane_u32(a) + b;
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator+=(Lane_U32 &a, Lane_U32 b) {
    a = a + b;
    return(a);
}

LANE_PUBLIC_DEC Lane_U32 operator-(Lane_U32 a, uint32_t b) {
    Lane_U32 r = a - lane_u32(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-(uint32_t a, Lane_U32 b) {
    Lane_U32 r = lane_u32(a) - b;
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator-=(Lane_U32 &a, Lane_U32 b) {
    a = a - b;
    return(a);
}

LANE_PUBLIC_DEC uint32_t extract(Lane_U32 a, uint32_t lane) {
    uint32_t r = ((uint32_t * )&a)[lane];
    return(r);
}

LANE_PUBLIC_DEC uint64_t horizontal_add(Lane_U32 a) {
    uint64_t r = 0;
    uint32_t *v = (uint32_t *)&a.v;
    for(uint32_t i = 0; (i < LANE_WIDTH); ++i) {
        r += v[i];
    }

    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator==(Lane_U32 a, uint32_t b) {
    Lane_U32 r = (a == lane_u32(b));
    return(r);
}

LANE_PUBLIC_DEC Lane_U32 operator!=(Lane_U32 a, uint32_t b) {
    Lane_U32 r = (a != lane_u32(b));
    return(r);
}

