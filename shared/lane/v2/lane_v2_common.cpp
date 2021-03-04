LANE_PUBLIC_DEC Lane_V2 lane_v2(V2 a) {
    Lane_V2 r;
    r.x = lane_f32(a.x);
    r.y = lane_f32(a.y);

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 lane_v2(Lane_F32 x, Lane_F32 y) {
    Lane_V2 r;
    r.x = x;
    r.y = y;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 lane_v2(float x, float y) {
    Lane_V2 r;
    r.x = lane_f32(x);
    r.y = lane_f32(y);

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 lane_v2(Lane_F32 a) {
    Lane_V2 r;
    r.x = a;
    r.y = a;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 lane_v2(float a) {
    Lane_V2 r;
    r.x = lane_f32(a);
    r.y = lane_f32(a);

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 normalise_or_zero(Lane_V2 a) {
    Lane_V2 r = lane_v2_zero();

    Lane_F32 len_sq = length_sq(a);
    Lane_U32 mask = (len_sq > lane_f32(0.0001f * 0.0001f));
    conditional_assign(mask, &r, a * (lane_f32(1.0f) / square_root(len_sq)));

    return(r);
}

Lane_V2 Lane_V2::operator=(V2 a) {
    *this = lane_v2(a);
    return(*this);
}

LANE_PUBLIC_DEC Lane_V2 lane_v2_zero(void) {
    Lane_V2 r;
    r.x = lane_f32_zero();
    r.y = lane_f32_zero();

    return(r);
}

// Mul
LANE_PUBLIC_DEC Lane_V2 operator*(Lane_V2 a, Lane_F32 b) {
    Lane_V2 r;
    r.x = a.x * b;
    r.y = a.y * b;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator*(Lane_F32 a, Lane_V2 b) {
    Lane_V2 r = (b * a);
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator*=(Lane_V2 &a, Lane_F32 b) {
    a = a * b;
    return(a);
}

LANE_PUBLIC_DEC Lane_V2 operator*(Lane_V2 a, float b) {
    Lane_V2 r;
    r.x = a.x * b;
    r.y = a.y * b;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator*(float a, Lane_V2 b) {
    Lane_V2 r = (b * a);
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator*=(Lane_V2 &a, float b) {
    a = a * b;
    return(a);
}

// Div
LANE_PUBLIC_DEC Lane_V2 operator/(Lane_V2 a, Lane_F32 b) {
    Lane_V2 r;
    r.x = a.x / b;
    r.y = a.y / b;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator/=(Lane_V2 &a, Lane_F32 b) {
    a = a / b;
    return(a);
}

LANE_PUBLIC_DEC Lane_V2 operator/(Lane_V2 a, float b) {
    Lane_V2 r;
    r.x = a.x / b;
    r.y = a.y / b;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator/=(Lane_V2 &a, float b) {
    a = a / b;
    return(a);
}


// Add
LANE_PUBLIC_DEC Lane_V2 operator+(Lane_V2 a, Lane_V2 b) {
    Lane_V2 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator+(Lane_V2 a, V2 b) {
    Lane_V2 r = a + lane_v2(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator+(V2 a, Lane_V2 b) {
    Lane_V2 r = lane_v2(a) + b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator+=(Lane_V2 &a, Lane_V2 b) {
    a = a + b;
    return(a);
}

// Subtract
LANE_PUBLIC_DEC Lane_V2 operator-(Lane_V2 a, Lane_V2 b) {
    Lane_V2 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator-(Lane_V2 a, V2 b) {
    Lane_V2 r = a - lane_v2(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator-(V2 a, Lane_V2 b) {
    Lane_V2 r = lane_v2(a) - b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 operator-(Lane_V2 a) {
    a = lane_v2_zero() - a;
    return(a);
}

LANE_PUBLIC_DEC Lane_V2 operator-=(Lane_V2 &a, Lane_V2 b) {
    a = a - b;
    return(a);
}

// And
LANE_PUBLIC_DEC Lane_V2 operator&(Lane_U32 a, Lane_V2 b) {
    Lane_V2 r;
    r.x = a & b.x;
    r.y = a & b.y;

    return(r);
}

// Comparison
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_V2 a, Lane_V2 b) {
    Lane_U32 r = ((a.x == b.x) & (a.y == b.y));

    return(r);
}

// Methods
LANE_PUBLIC_DEC Lane_F32 inner(Lane_V2 a, Lane_V2 b) {
    Lane_F32 r = ((a.x * b.x) + (a.y * b.y));
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 hadamard(Lane_V2 a, Lane_V2 b) {
    Lane_V2 r;
    r.x = a.x * b.x;
    r.y = a.y * b.y;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 length_sq(Lane_V2 a) {
    Lane_F32 r = inner(a, a);
    return(r);
}

LANE_PUBLIC_DEC Lane_F32 length(Lane_V2 a) {
    Lane_F32 r = square_root(length_sq(a)); // TODO: Use wide square root?
    return(r);
}

LANE_PUBLIC_DEC Lane_V2 normalise(Lane_V2 a) {
    Lane_V2 r = a * (1.0f / length(a));
    return(r);
}

LANE_PUBLIC_DEC V2 extract(Lane_V2 a, uint32_t lane) {
    V2 r = ((V2 *)&a)[lane];
    return(r);
}

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_V2 *dst, Lane_V2 src) {
    conditional_assign(mask, &dst->x, src.x);
    conditional_assign(mask, &dst->y, src.y);
}

LANE_PUBLIC_DEC V2 lane_horizontal_add(Lane_V2 a) {
    V2 r;
    r.x = lane_horizontal_add(a.x);
    r.y = lane_horizontal_add(a.y);

    return(r);
}

LANE_PUBLIC_DEC Lane_V2 gather_v2_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_V2 r;
    // TODO: Not most efficient way - could write a custom V2 gather for each lane width.
    r.x = gather_f32_internal((float *)ptr + 0, stride, indices);
    r.y = gather_f32_internal((float *)ptr + 1, stride, indices);

    return(r);
}
