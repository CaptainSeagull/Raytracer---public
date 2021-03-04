struct M4x4 {
    union {
        F32 e[4][4]; // [row][col]
        F32 f[16];
        V4  rows[4];
    };
};
static_assert(sizeof(M4x4) == sizeof(F32) * 16, "M4x4 size wrong");

internal V4 extract_row(M4x4 m, Int row) {
    ASSERT(row >= 0 && row < 4);
    V4 r = v4(m.e[row][0], m.e[row][1], m.e[row][2], m.e[row][3]);

    return(r);
}

internal V4 extract_col(M4x4 m, Int col) {
    ASSERT(col >= 0 && col < 4);
    V4 r = v4(m.e[0][col], m.e[1][col], m.e[2][col], m.e[3][col]);

    return(r);
}

internal V3 transform(M4x4 a, V4 b) {
    V3 r;
    r.x = b.x * a.e[0][0] + b.y * a.e[0][1] + b.z * a.e[0][2] + b.w * a.e[0][3];
    r.y = b.x * a.e[1][0] + b.y * a.e[1][1] + b.z * a.e[1][2] + b.w * a.e[1][3];
    r.z = b.x * a.e[2][0] + b.y * a.e[2][1] + b.z * a.e[2][2] + b.w * a.e[2][3];

    return(r);
}

internal V3 operator*(M4x4 a, V3 b) {
    V3 r = transform(a, v4(b.x, b.y, b.z, 1.0f));
    return(r);
}

internal V3 operator*(M4x4 a, V4 b) {
    V3 r = transform(a, b);
    return(r);
}

internal M4x4 operator*(M4x4 a, M4x4 b) {
    M4x4 r;

#if 0
    for(Int row = 0; (row < 4); ++row) {
        for(Int col = 0; (col < 4); ++col) {
            r[row][col] = 0.0f;
            for(Int i = 0; (i < 4); ++i) {
                r.e[row][col] += a.e[row][i] * b.e[i][col];
            }
        }
    }
#else
    r.e[0][0] = a.e[0][0] * b.e[0][0] + a.e[0][1] * b.e[1][0] + a.e[0][2] * b.e[2][0] + a.e[0][3] * b.e[3][0];
    r.e[0][1] = a.e[0][0] * b.e[0][1] + a.e[0][1] * b.e[1][1] + a.e[0][2] * b.e[2][1] + a.e[0][3] * b.e[3][1];
    r.e[0][2] = a.e[0][0] * b.e[0][2] + a.e[0][1] * b.e[1][2] + a.e[0][2] * b.e[2][2] + a.e[0][3] * b.e[3][2];
    r.e[0][3] = a.e[0][0] * b.e[0][3] + a.e[0][1] * b.e[1][3] + a.e[0][2] * b.e[2][3] + a.e[0][3] * b.e[3][3];

    r.e[1][0] = a.e[1][0] * b.e[0][0] + a.e[1][1] * b.e[1][0] + a.e[1][2] * b.e[2][0] + a.e[1][3] * b.e[3][0];
    r.e[1][1] = a.e[1][0] * b.e[0][1] + a.e[1][1] * b.e[1][1] + a.e[1][2] * b.e[2][1] + a.e[1][3] * b.e[3][1];
    r.e[1][2] = a.e[1][0] * b.e[0][2] + a.e[1][1] * b.e[1][2] + a.e[1][2] * b.e[2][2] + a.e[1][3] * b.e[3][2];
    r.e[1][3] = a.e[1][0] * b.e[0][3] + a.e[1][1] * b.e[1][3] + a.e[1][2] * b.e[2][3] + a.e[1][3] * b.e[3][3];

    r.e[2][0] = a.e[2][0] * b.e[0][0] + a.e[2][1] * b.e[1][0] + a.e[2][2] * b.e[2][0] + a.e[2][3] * b.e[3][0];
    r.e[2][1] = a.e[2][0] * b.e[0][1] + a.e[2][1] * b.e[1][1] + a.e[2][2] * b.e[2][1] + a.e[2][3] * b.e[3][1];
    r.e[2][2] = a.e[2][0] * b.e[0][2] + a.e[2][1] * b.e[1][2] + a.e[2][2] * b.e[2][2] + a.e[2][3] * b.e[3][2];
    r.e[2][3] = a.e[2][0] * b.e[0][3] + a.e[2][1] * b.e[1][3] + a.e[2][2] * b.e[2][3] + a.e[2][3] * b.e[3][3];

    r.e[3][0] = a.e[3][0] * b.e[0][0] + a.e[3][1] * b.e[1][0] + a.e[3][2] * b.e[2][0] + a.e[3][3] * b.e[3][0];
    r.e[3][1] = a.e[3][0] * b.e[0][1] + a.e[3][1] * b.e[1][1] + a.e[3][2] * b.e[2][1] + a.e[3][3] * b.e[3][1];
    r.e[3][2] = a.e[3][0] * b.e[0][2] + a.e[3][1] * b.e[1][2] + a.e[3][2] * b.e[2][2] + a.e[3][3] * b.e[3][2];
    r.e[3][3] = a.e[3][0] * b.e[0][3] + a.e[3][1] * b.e[1][3] + a.e[3][2] * b.e[2][3] + a.e[3][3] * b.e[3][3];
#endif

    return(r);
}

internal M4x4 operator*=(M4x4 &a, M4x4 b) {
    a = a * b;
    return(a);
}

internal M4x4 iden(Void) {
    M4x4 r;
    r.e[0][0] = 1; r.e[0][1] = 0; r.e[0][2] = 0; r.e[0][3] = 0;
    r.e[1][0] = 0; r.e[1][1] = 1; r.e[1][2] = 0; r.e[1][3] = 0;
    r.e[2][0] = 0; r.e[2][1] = 0; r.e[2][2] = 1; r.e[2][3] = 0;
    r.e[3][0] = 0; r.e[3][1] = 0; r.e[3][2] = 0; r.e[3][3] = 1;

    return(r);
}

internal F32 crappy_radian_clamp(F32 a) {
    while(a <  0              ) { a += LANE_PI32 * 2; }
    while(a >= (LANE_PI32 * 2)) { a -= LANE_PI32 * 2; }

    return(a);
}

internal M4x4 x_rotation(F32 angle) {
    angle = crappy_radian_clamp(angle);
    F32 s = sinf(angle);
    F32 c = cosf(angle);

    M4x4 r;
    r.e[0][0] = 1; r.e[0][1] = 0; r.e[0][2] =  0; r.e[0][3] = 0;
    r.e[1][0] = 0; r.e[1][1] = c; r.e[1][2] = -s; r.e[1][3] = 0;
    r.e[2][0] = 0; r.e[2][1] = s; r.e[2][2] =  c; r.e[2][3] = 0;
    r.e[3][0] = 0; r.e[3][1] = 0; r.e[3][2] =  0; r.e[3][3] = 1;

    return(r);
}

internal M4x4 y_rotation(F32 angle) {
    angle = crappy_radian_clamp(angle);
    F32 s = sinf(angle);
    F32 c = cosf(angle);

    M4x4 r;
    r.e[0][0] =  c; r.e[0][1] = 0; r.e[0][2] = s; r.e[0][3] = 0;
    r.e[1][0] =  0; r.e[1][1] = 1; r.e[1][2] = 0; r.e[1][3] = 0;
    r.e[2][0] = -s; r.e[2][1] = 0; r.e[2][2] = c; r.e[2][3] = 0;
    r.e[3][0] =  0; r.e[3][1] = 0; r.e[3][2] = 0; r.e[3][3] = 1;

    return(r);
}

internal M4x4 z_rotation(F32 angle) {
    angle = crappy_radian_clamp(angle);
    F32 s = sinf(angle);
    F32 c = cosf(angle);

    M4x4 r;
    r.e[0][0] = c; r.e[0][1] = -s; r.e[0][2] = 0; r.e[0][3] = 0;
    r.e[1][0] = s; r.e[1][1] =  c; r.e[1][2] = 0; r.e[1][3] = 0;
    r.e[2][0] = 0; r.e[2][1] =  0; r.e[2][2] = 1; r.e[2][3] = 0;
    r.e[3][0] = 0; r.e[3][1] =  0; r.e[3][2] = 0; r.e[3][3] = 1;

    return(r);
}

internal M4x4 transpose(M4x4 m) {
    M4x4 r;

    // TODO: Do this without loops.
    for(Int i = 0; (i < 4); ++i) {
        for(Int j = 0; (j < 4); ++j) {
            r.e[i][j] = m.e[j][i];
        }
    }

    return(r);
}

internal M4x4 projection(F32 aspect_ratio, F32 focal_length) {
    F32 a = 1.0f;
    F32 b = aspect_ratio;
    F32 c = 1.0f / focal_length;

    M4x4 r;
    r.e[0][0] = a; r.e[0][1] = 0; r.e[0][2] = 0; r.e[0][3] = 0;
    r.e[1][0] = 0; r.e[1][1] = b; r.e[1][2] = 0; r.e[1][3] = 0;
    r.e[2][0] = 0; r.e[2][1] = 0; r.e[2][2] = 1; r.e[2][3] = 0;
    r.e[3][0] = 0; r.e[3][1] = 0; r.e[3][2] = c; r.e[3][3] = 1;

    return(r);
}

internal M4x4 columns3x3(V3 x, V3 y, V3 z) {
    M4x4 r;
    r.e[0][0] = x.x; r.e[0][1] = y.x; r.e[0][2] = z.x; r.e[0][3] = 0;
    r.e[1][0] = x.y; r.e[1][1] = y.y; r.e[1][2] = z.y; r.e[1][3] = 0;
    r.e[2][0] = x.z; r.e[2][1] = y.z; r.e[2][2] = z.z; r.e[2][3] = 0;
    r.e[3][0] = 0;   r.e[3][1] = 0;   r.e[3][2] = 0;   r.e[3][3] = 1;

    return(r);
}

internal M4x4 rows3x3(V3 x, V3 y, V3 z) {
    M4x4 r;
    r.e[0][0] = x.x; r.e[0][1] = x.y; r.e[0][2] = x.z; r.e[0][3] = 0;
    r.e[1][0] = y.x; r.e[1][1] = y.y; r.e[1][2] = y.z; r.e[1][3] = 0;
    r.e[2][0] = z.x; r.e[2][1] = z.y; r.e[2][2] = z.z; r.e[2][3] = 0;
    r.e[3][0] = 0;   r.e[3][1] = 0;   r.e[3][2] = 0;   r.e[3][3] = 1;

    return(r);
}

internal M4x4 translate(M4x4 a, V3 t) {
    a.e[0][3] += t.x;
    a.e[1][3] += t.y;
    a.e[2][3] += t.z;

    return(a);
}

internal M4x4 scale(M4x4 a, V3 s) {
    a.e[0][0] *= s.x;
    a.e[1][1] *= s.y;
    a.e[2][2] *= s.z;
    return(a);
}

internal V3 extract_scale(M4x4 a) {
    V3 r;
    r.x = a.e[0][0];
    r.y = a.e[1][1];
    r.z = a.e[2][2];

    return(r);
}

internal M4x4 camera_transform(V3 x, V3 y, V3 z, V3 p) {
    M4x4 r = rows3x3(x, y, z);
    r = translate(r, -(r * p));

    return(r);
}
