LANE_PUBLIC_DEC V2 v2(V2 a) {
    V2 r;
    r.x = a.x;
    r.y = a.y;

    return(r);
}

LANE_PUBLIC_DEC V2 v2(float x, float y) {
    V2 r;
    r.x = x;
    r.y = y;

    return(r);
}

LANE_PUBLIC_DEC V2 v2(float a) {
    V2 r;
    r.x = a;
    r.y = a;

    return(r);
}

LANE_PUBLIC_DEC V2 normalise_or_zero(V2 a) {
    V2 r = {};

    float len_sq = length_sq(a);
    if(len_sq > float(0.0001f * 0.0001f)) {
        r = a * (float(1.0f) / square_root(len_sq));
    }

    return(r);
}

// Mul
LANE_PUBLIC_DEC V2 operator*(V2 a, float b) {
    V2 r;
    r.x = a.x * b;
    r.y = a.y * b;

    return(r);
}

LANE_PUBLIC_DEC V2 operator*(float a, V2 b) {
    V2 r = (b * a);
    return(r);
}

LANE_PUBLIC_DEC V2 operator*=(V2 &a, float b) {
    a = a * b;
    return(a);
}

// Div
LANE_PUBLIC_DEC V2 operator/(float a, V2 b) {
    V2 r;
    r.x = a / b.x;
    r.y = a / b.y;

    return(r);
}

LANE_PUBLIC_DEC V2 operator/(V2 a, float b) {
    V2 r;
    r.x = a.x / b;
    r.y = a.y / b;

    return(r);
}

LANE_PUBLIC_DEC V2 operator/=(V2 &a, float b) {
    a = a / b;
    return(a);
}

// Add
LANE_PUBLIC_DEC V2 operator+(V2 a, V2 b) {
    V2 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;

    return(r);
}

LANE_PUBLIC_DEC V2 operator+=(V2 &a, V2 b) {
    a = a + b;
    return(a);
}

// Subtract
LANE_PUBLIC_DEC V2 operator-(V2 a, V2 b) {
    V2 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;

    return(r);
}

LANE_PUBLIC_DEC V2 operator-(V2 a) {
    a = v2(0) - a;
    return(a);
}

LANE_PUBLIC_DEC V2 operator-=(V2 &a, V2 b) {
    a = a - b;
    return(a);
}

// And
LANE_PUBLIC_DEC V2 operator&(uint32_t a, V2 b) {
    V2 r;
    r.x = a & (uint32_t)b.x;
    r.y = a & (uint32_t)b.y;

    return(r);
}

// Methods
LANE_PUBLIC_DEC float inner(V2 a, V2 b) {
    float r = ((a.x * b.x) + (a.y * b.y));
    return(r);
}

LANE_PUBLIC_DEC V2 hadamard(V2 a, V2 b) {
    V2 r;
    r.x = a.x * b.x;
    r.y = a.y * b.y;

    return(r);
}

LANE_PUBLIC_DEC float length_sq(V2 a) {
    float r = inner(a, a);
    return(r);
}

LANE_PUBLIC_DEC float length(V2 a) {
    float r = square_root(length_sq(a)); // TODO: Use wide square root?
    return(r);
}

LANE_PUBLIC_DEC V2 normalise(V2 a) {
    V2 r = a * (1.0f / length(a));
    return(r);
}
