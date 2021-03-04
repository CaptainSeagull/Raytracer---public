LANE_PUBLIC_DEC Lane_V3 lane_v3(V3 a) {
    Lane_V3 r;
    r.x = lane_f32(a.x);
    r.y = lane_f32(a.y);
    r.z = lane_f32(a.z);

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 lane_v3(Lane_F32 x, Lane_F32 y, Lane_F32 z) {
    Lane_V3 r;
    r.x = x;
    r.y = y;
    r.z = z;

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 lane_v3(float x, float y, float z) {
    Lane_V3 r;
    r.x = x;
    r.y = y;
    r.z = z;

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 lane_v3(float a) {
    Lane_V3 r;
    r.x = lane_f32(a);
    r.y = lane_f32(a);
    r.z = lane_f32(a);

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 lane_v3(Lane_F32 a) {
    Lane_V3 r;
    r.x = a;
    r.y = a;
    r.z = a;

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 normalise_or_zero(Lane_V3 a) {
    Lane_V3 r = lane_v3_zero();

    Lane_F32 len_sq = length_sq(a);
    Lane_U32 mask = (len_sq > lane_f32(0.0001f * 0.0001f));
    conditional_assign(mask, &r, a * (lane_f32(1.0f) / square_root(len_sq)));

    return(r);
}

Lane_V3 Lane_V3::operator=(V3 a) {
    *this = lane_v3(a);
    return(*this);
}

LANE_PUBLIC_DEC Lane_V3 lane_v3_zero(void) {
    Lane_V3 r;
    r.x = lane_f32_zero();
    r.y = lane_f32_zero();
    r.z = lane_f32_zero();

    return(r);
}

// Mul
LANE_PUBLIC_DEC Lane_V3 operator*(Lane_V3 a, Lane_F32 b) {
    Lane_V3 r;
    r.x = a.x * b;
    r.y = a.y * b;
    r.z = a.z * b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator*(Lane_F32 a, Lane_V3 b) {
    Lane_V3 r = (b * a);
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator*=(Lane_V3 &a, Lane_F32 b) {
    a = a * b;
    return(a);
}

LANE_PUBLIC_DEC Lane_V3 operator*(Lane_V3 a, float b) {
    Lane_V3 r;
    r.x = a.x * b;
    r.y = a.y * b;
    r.z = a.z * b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator*(float a, Lane_V3 b) {
    Lane_V3 r = (b * a);
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator*=(Lane_V3 &a, float b) {
    a = a * b;
    return(a);
}


// Div
LANE_PUBLIC_DEC Lane_V3 operator/(Lane_V3 a, Lane_F32 b) {
    Lane_V3 r;
    r.x = a.x / b;
    r.y = a.y / b;
    r.z = a.z / b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator/=(Lane_V3 &a, Lane_F32 b) {
    a = a / b;
    return(a);
}

LANE_PUBLIC_DEC Lane_V3 operator/(Lane_V3 a, float b) {
    Lane_V3 r;
    r.x = a.x / b;
    r.y = a.y / b;
    r.z = a.z / b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator/=(Lane_V3 &a, float b) {
    a = a / b;
    return(a);
}


// Add
LANE_PUBLIC_DEC Lane_V3 operator+(Lane_V3 a, Lane_V3 b) {
    Lane_V3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator+(Lane_V3 a, V3 b) {
    Lane_V3 r = a + lane_v3(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator+(V3 a, Lane_V3 b) {
    Lane_V3 r = lane_v3(a) + b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator+=(Lane_V3 &a, Lane_V3 b) {
    a = a + b;
    return(a);
}

// Subtract
LANE_PUBLIC_DEC Lane_V3 operator-(Lane_V3 a, Lane_V3 b) {
    Lane_V3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator-(Lane_V3 a, V3 b) {
    Lane_V3 r = a - lane_v3(b);
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator-(V3 a, Lane_V3 b) {
    Lane_V3 r = lane_v3(a) - b;
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 operator-(Lane_V3 a) {
    a = lane_v3_zero() - a;
    return(a);
}

LANE_PUBLIC_DEC Lane_V3 operator-=(Lane_V3 &a, Lane_V3 b) {
    a = a - b;
    return(a);
}

// And
LANE_PUBLIC_DEC Lane_V3 operator&(Lane_U32 a, Lane_V3 b) {
    Lane_V3 r;
    r.x = a & b.x;
    r.y = a & b.y;
    r.z = a & b.z;

    return(r);
}

// Comparison
LANE_PUBLIC_DEC Lane_U32 operator==(Lane_V3 a, Lane_V3 b) {
    Lane_U32 r = ((a.x == b.x) & (a.y == b.y) & (a.z == b.z));

    return(r);
}

// Methods
LANE_PUBLIC_DEC Lane_F32 inner(Lane_V3 a, Lane_V3 b) {
    Lane_F32 r = ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 hadamard(Lane_V3 a, Lane_V3 b) {
    Lane_V3 r;
    r.x = a.x * b.x;
    r.y = a.y * b.y;
    r.z = a.z * b.z;

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 cross(Lane_V3 a, Lane_V3 b) {
    Lane_V3 r;
    r.x = ((a.y * b.z) - (a.z * b.y));
    r.y = ((a.z * b.x) - (a.x * b.z));
    r.z = ((a.x * b.y) - (a.y * b.x));

    return(r);
}

LANE_PUBLIC_DEC Lane_F32 length_sq(Lane_V3 a) {
    Lane_F32 r = inner(a, a);
    return(r);
}

LANE_PUBLIC_DEC Lane_F32 length(Lane_V3 a) {
    Lane_F32 r = square_root(length_sq(a)); // TODO: Use wide square root?
    return(r);
}

LANE_PUBLIC_DEC Lane_V3 normalise(Lane_V3 a) {
    Lane_V3 r = a * (1.0f / length(a));
    return(r);
}

LANE_PUBLIC_DEC V3 extract(Lane_V3 a, uint32_t lane) {
    V3 r = ((V3 *)&a)[lane];
    return(r);
}

LANE_PUBLIC_DEC void conditional_assign(Lane_U32 mask, Lane_V3 *dst, Lane_V3 src) {
    conditional_assign(mask, &dst->x, src.x);
    conditional_assign(mask, &dst->y, src.y);
    conditional_assign(mask, &dst->z, src.z);
}

LANE_PUBLIC_DEC V3 lane_horizontal_add(Lane_V3 a) {
    V3 r;
    r.x = lane_horizontal_add(a.x);
    r.y = lane_horizontal_add(a.y);
    r.z = lane_horizontal_add(a.z);

    return(r);
}

LANE_PUBLIC_DEC Lane_V3 gather_v3_internal(void *ptr, uint64_t stride, Lane_U32 indices) {
    Lane_V3 r;
    // TODO: Not most efficient way - could write a custom V3 gather for each lane width.
    r.x = gather_f32_internal((float *)ptr + 0, stride, indices);
    r.y = gather_f32_internal((float *)ptr + 1, stride, indices);
    r.z = gather_f32_internal((float *)ptr + 2, stride, indices);

    return(r);
}
