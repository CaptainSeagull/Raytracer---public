LANE_PUBLIC_DEC Lane_V4 lane_v4(V4 a) {
    Lane_V4 r;
    r.x = lane_f32(a.x);
    r.y = lane_f32(a.y);
    r.z = lane_f32(a.z);
    r.w = lane_f32(a.w);

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 lane_v4(Lane_F32 x, Lane_F32 y, Lane_F32 z, Lane_F32 w) {
    Lane_V4 r;
    r.x = x;
    r.y = y;
    r.z = z;
    r.w = w;

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 lane_v4(float x, float y, float z, float w) {
    Lane_V4 r;
    r.x = x;
    r.y = y;
    r.z = z;
    r.w = w;

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 lane_v4(Lane_F32 a) {
    Lane_V4 r;
    r.x = a;
    r.y = a;
    r.z = a;
    r.w = a;

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 lane_v4(float a) {
    Lane_V4 r;
    r.x = lane_f32(a);
    r.y = lane_f32(a);
    r.z = lane_f32(a);
    r.w = lane_f32(a);

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 normalise_or_zero(Lane_V4 a) {
    Lane_V4 r = lane_v4_zero();

    Lane_F32 len_sq = length_sq(a);
    Lane_U32 mask = (len_sq > lane_f32(0.0001f * 0.0001f));
    conditional_assign(mask, &r, a * (lane_f32(1.0f) / square_root(len_sq)));

    return(r);
}

Lane_V4 Lane_V4::operator=(V4 v) {
    *this = lane_v4(v);
    return(*this);
}

LANE_PUBLIC_DEC Lane_V4 lane_v4_zero(void) {
    Lane_V4 r;
    r.x = lane_f32_zero();
    r.y = lane_f32_zero();
    r.z = lane_f32_zero();
    r.w = lane_f32_zero();

    return(r);
}

// Mul
LANE_PUBLIC_DEC Lane_V4 operator*(Lane_V4 a, Lane_F32 b) {
    Lane_V4 r;
    r.x = a.x * b;
    r.y = a.y * b;
    r.z = a.z * b;
    r.w = a.w * b;

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator*(Lane_F32 a, Lane_V4 b) {
    Lane_V4 r = (b * a);
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator*=(Lane_V4 &a, Lane_F32 b) {
    a = a * b;
    return(a);
}

LANE_PUBLIC_DEC Lane_V4 operator*(Lane_V4 a, float b) {
    Lane_V4 r;
    r.x = a.x * b;
    r.y = a.y * b;
    r.z = a.z * b;
    r.w = a.w * b;

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator*(float a, Lane_V4 b) {
    Lane_V4 r = (b * a);
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator*=(Lane_V4 &a, float b) {
    a = a * b;
    return(a);
}


// Div
LANE_PUBLIC_DEC Lane_V4 operator/(Lane_V4 a, Lane_F32 b) {
    Lane_V4 r;
    r.x = a.x / b;
    r.y = a.y / b;
    r.z = a.z / b;
    r.w = a.w / b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator/=(Lane_V4 &a, Lane_F32 b) {
    a = a / b;
    return(a);
}

LANE_PUBLIC_DEC Lane_V4 operator/(Lane_V4 a, float b) {
    Lane_V4 r;
    r.x = a.x / b;
    r.y = a.y / b;
    r.z = a.z / b;
    r.w = a.w / b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator/=(Lane_V4 &a, float b) {
    a = a / b;
    return(a);
}

// Add
LANE_PUBLIC_DEC Lane_V4 operator+(Lane_V4 a, Lane_V4 b) {
    Lane_V4 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    r.w = a.w + b.w;

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator+(Lane_V4 a, V4 b) {
    Lane_V4 r = a + lane_v4(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator+(V4 a, Lane_V4 b) {
    Lane_V4 r = lane_v4(a) + b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator+=(Lane_V4 &a, Lane_V4 b) {
    a = a + b;
    return(a);
}

// Subtract
LANE_PUBLIC_DEC Lane_V4 operator-(Lane_V4 a, Lane_V4 b) {
    Lane_V4 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    r.w = a.w - b.w;

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator-(Lane_V4 a, V4 b) {
    Lane_V4 r = a - lane_v4(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator-(V4 a, Lane_V4 b) {
    Lane_V4 r = lane_v4(a) - b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 operator-(Lane_V4 a) {
    a = lane_v4_zero() - a;
    return(a);
}

LANE_PUBLIC_DEC Lane_V4 operator-=(Lane_V4 &a, Lane_V4 b) {
    a = a - b;
    return(a);
}

// And
LANE_PUBLIC_DEC Lane_V4 operator&(Lane_U32 a, Lane_V4 b) {
    Lane_V4 r;
    r.x = a & b.x;
    r.y = a & b.y;
    r.z = a & b.z;
    r.w = a & b.w;

    return(r);
}

// Comparison
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_V4 a, Lane_V4 b) {
    Lane_U32 r = ((a.x == b.x) & (a.y == b.y) & (a.z == b.z) & (a.w == b.w));

    return(r);
}

// Methods
LANE_PUBLIC_DEC Lane_F32 inner(Lane_V4 a, Lane_V4 b) {
    Lane_F32 r = ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 hadamard(Lane_V4 a, Lane_V4 b) {
    Lane_V4 r;
    r.x = a.x * b.x;
    r.y = a.y * b.y;
    r.z = a.z * b.z;
    r.w = a.w * b.w;

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 length_sq(Lane_V4 a) {
    Lane_F32 r = inner(a, a);
    return(r);
}

LANE_PUBLIC_DEC Lane_F32 length(Lane_V4 a) {
    Lane_F32 r = square_root(length_sq(a)); // TODO: Use wide square root?
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 normalise(Lane_V4 a) {
    Lane_V4 r = a * (1.0f / length(a));
    return(r);
}

LANE_PUBLIC_DEC V4 extract(Lane_V4 a, uint32_t lane) {
    V4 r = ((V4 *)&a)[lane];
    return(r);
}

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_V4 *dst, Lane_V4 src) {
    conditional_assign(mask, &dst->x, src.x);
    conditional_assign(mask, &dst->y, src.y);
    conditional_assign(mask, &dst->z, src.z);
    conditional_assign(mask, &dst->w, src.w);
}

LANE_PUBLIC_DEC V4 horizontal_add(Lane_V4 a) {
    V4 r;
    r.x = horizontal_add(a.x);
    r.y = horizontal_add(a.y);
    r.z = horizontal_add(a.z);
    r.w = horizontal_add(a.w);

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 gather_v4_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_V4 r;
    // TODO: Not most efficient way - could write a custom V4 gather for each lane width.
    r.x = gather_f32_internal((float *)ptr + 0, stride, indices);
    r.y = gather_f32_internal((float *)ptr + 1, stride, indices);
    r.z = gather_f32_internal((float *)ptr + 2, stride, indices);
    r.w = gather_f32_internal((float *)ptr + 3, stride, indices);

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 conditional_gather_v4_internal(Lane_U32 mask, void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_V4 r;
    // TODO: Not most efficient way - could write a custom V4 gather for each lane width.
    r.x = gather_f32_internal((float *)ptr + 0, stride, indices);
    r.y = gather_f32_internal((float *)ptr + 1, stride, indices);
    r.z = gather_f32_internal((float *)ptr + 2, stride, indices);
    r.w = gather_f32_internal((float *)ptr + 3, stride, indices);

    return(r);
}

LANE_PUBLIC_DEC Lane_V4 lerp(Lane_F32 t, Lane_V4 a, Lane_V4 b) {
    Lane_V4 r = (lane_f32(1.0f) - t) * a + t * b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V4 lerp(float t, Lane_V4 a, Lane_V4 b) {
    Lane_V4 r = (lane_f32(1.0f) - lane_f32(t)) * a + t * b;
    return(r);
}
