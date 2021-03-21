struct M4x4 {
    union {
        F32 e[4][4]; // [row][col]
        F32 f[16];
        V4  rows[4];
    };
};
static_assert(sizeof(M4x4) == sizeof(F32) * 16, "M4x4 size wrong");

internal M4x4 m4x4(F32 a, F32 b, F32 c, F32 d,
                   F32 e, F32 f, F32 g, F32 h,
                   F32 i, F32 j, F32 k, F32 l,
                   F32 m, F32 n, F32 o, F32 p) {
    M4x4 r;
    r.f[ 0] = a; r.f[ 1] = b; r.f[ 2] = c; r.f[ 3] = d;
    r.f[ 4] = e; r.f[ 5] = f; r.f[ 6] = g; r.f[ 7] = h;
    r.f[ 8] = i; r.f[ 9] = j; r.f[10] = k; r.f[11] = l;
    r.f[12] = m; r.f[13] = n; r.f[14] = o; r.f[15] = p;

    return(r);
}

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
    r = m4x4(a.e[0][0] * b.e[0][0] + a.e[0][1] * b.e[1][0] + a.e[0][2] * b.e[2][0] + a.e[0][3] * b.e[3][0],
             a.e[0][0] * b.e[0][1] + a.e[0][1] * b.e[1][1] + a.e[0][2] * b.e[2][1] + a.e[0][3] * b.e[3][1],
             a.e[0][0] * b.e[0][2] + a.e[0][1] * b.e[1][2] + a.e[0][2] * b.e[2][2] + a.e[0][3] * b.e[3][2],
             a.e[0][0] * b.e[0][3] + a.e[0][1] * b.e[1][3] + a.e[0][2] * b.e[2][3] + a.e[0][3] * b.e[3][3],

             a.e[1][0] * b.e[0][0] + a.e[1][1] * b.e[1][0] + a.e[1][2] * b.e[2][0] + a.e[1][3] * b.e[3][0],
             a.e[1][0] * b.e[0][1] + a.e[1][1] * b.e[1][1] + a.e[1][2] * b.e[2][1] + a.e[1][3] * b.e[3][1],
             a.e[1][0] * b.e[0][2] + a.e[1][1] * b.e[1][2] + a.e[1][2] * b.e[2][2] + a.e[1][3] * b.e[3][2],
             a.e[1][0] * b.e[0][3] + a.e[1][1] * b.e[1][3] + a.e[1][2] * b.e[2][3] + a.e[1][3] * b.e[3][3],

             a.e[2][0] * b.e[0][0] + a.e[2][1] * b.e[1][0] + a.e[2][2] * b.e[2][0] + a.e[2][3] * b.e[3][0],
             a.e[2][0] * b.e[0][1] + a.e[2][1] * b.e[1][1] + a.e[2][2] * b.e[2][1] + a.e[2][3] * b.e[3][1],
             a.e[2][0] * b.e[0][2] + a.e[2][1] * b.e[1][2] + a.e[2][2] * b.e[2][2] + a.e[2][3] * b.e[3][2],
             a.e[2][0] * b.e[0][3] + a.e[2][1] * b.e[1][3] + a.e[2][2] * b.e[2][3] + a.e[2][3] * b.e[3][3],

             a.e[3][0] * b.e[0][0] + a.e[3][1] * b.e[1][0] + a.e[3][2] * b.e[2][0] + a.e[3][3] * b.e[3][0],
             a.e[3][0] * b.e[0][1] + a.e[3][1] * b.e[1][1] + a.e[3][2] * b.e[2][1] + a.e[3][3] * b.e[3][1],
             a.e[3][0] * b.e[0][2] + a.e[3][1] * b.e[1][2] + a.e[3][2] * b.e[2][2] + a.e[3][3] * b.e[3][2],
             a.e[3][0] * b.e[0][3] + a.e[3][1] * b.e[1][3] + a.e[3][2] * b.e[2][3] + a.e[3][3] * b.e[3][3]);
#endif

    return(r);
}

internal M4x4 operator*=(M4x4 &a, M4x4 b) {
    a = a * b;
    return(a);
}

internal M4x4 iden(Void) {
    M4x4 r = m4x4(1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1);
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

    M4x4 r = m4x4(1, 0,  0, 0,
                  0, c, -s, 0,
                  0, s,  c, 0,
                  0, 0,  0, 1);

    return(r);
}

internal M4x4 y_rotation(F32 angle) {
    angle = crappy_radian_clamp(angle);
    F32 s = sinf(angle);
    F32 c = cosf(angle);

    M4x4 r = m4x4( c,  0, s, 0,
                   0,  1, 0, 0,
                   -s, 0, c, 0,
                   0,  0, 0, 1);

    return(r);
}

internal M4x4 z_rotation(F32 angle) {
    angle = crappy_radian_clamp(angle);
    F32 s = sinf(angle);
    F32 c = cosf(angle);

    M4x4 r = m4x4(c, -s, 0, 0,
                  s,  c, 0, 0,
                  0,  0, 1, 0,
                  0,  0, 0, 1);

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

internal M4x4 columns3x3(V3 x, V3 y, V3 z) {
    M4x4 r = m4x4(x.x, y.x, z.x, 0,
                  x.y, y.y, z.y, 0,
                  x.z, y.z, z.z, 0,
                  0,   0,   0,   1);

    return(r);
}

internal M4x4 rows3x3(V3 x, V3 y, V3 z) {
    M4x4 r = m4x4(x.x, x.y, x.z, 0,
                  y.x, y.y, y.z, 0,
                  z.x, z.y, z.z, 0,
                  0,   0,   0,   1);

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



struct Lane_M4x4 {
    union {
        Lane_F32 e[4][4]; // [row][col]
        Lane_F32 f[16];
        Lane_V4  rows[4];
    };
};
static_assert(sizeof(Lane_M4x4) == sizeof(F32) * 16 * LANE_WIDTH, "Lane_M4x4 size wrong");

internal Lane_M4x4 lane_m4x4(M4x4 a) {
    Lane_M4x4 r;
    r.rows[0] = lane_v4(a.rows[0]);
    r.rows[1] = lane_v4(a.rows[1]);
    r.rows[2] = lane_v4(a.rows[2]);
    r.rows[3] = lane_v4(a.rows[3]);

    return(r);
}

internal Lane_V4 extract_row(Lane_M4x4 m, Int row) {
    ASSERT(row >= 0 && row < 4);
    Lane_V4 r = lane_v4(m.e[row][0], m.e[row][1], m.e[row][2], m.e[row][3]);

    return(r);
}

internal Lane_V4 extract_col(Lane_M4x4 m, Int col) {
    ASSERT(col >= 0 && col < 4);
    Lane_V4 r = lane_v4(m.e[0][col], m.e[1][col], m.e[2][col], m.e[3][col]);

    return(r);
}

internal Lane_V3 transform(Lane_M4x4 a, Lane_V4 b) {
    Lane_V3 r;
    r.x = b.x * a.e[0][0] + b.y * a.e[0][1] + b.z * a.e[0][2] + b.w * a.e[0][3];
    r.y = b.x * a.e[1][0] + b.y * a.e[1][1] + b.z * a.e[1][2] + b.w * a.e[1][3];
    r.z = b.x * a.e[2][0] + b.y * a.e[2][1] + b.z * a.e[2][2] + b.w * a.e[2][3];

    return(r);
}

internal Lane_V3 operator*(Lane_M4x4 a, Lane_V3 b) {
    Lane_V3 r = transform(a, lane_v4(b.x, b.y, b.z, lane_f32(1.0f)));
    return(r);
}

internal Lane_V3 operator*(Lane_M4x4 a, Lane_V4 b) {
    Lane_V3 r = transform(a, b);
    return(r);
}

internal Lane_M4x4 operator*(Lane_M4x4 a, Lane_M4x4 b) {
    Lane_M4x4 r;

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

internal Lane_M4x4 operator*=(Lane_M4x4 &a, Lane_M4x4 b) {
    a = a * b;
    return(a);
}

internal Lane_M4x4 lane_iden(Void) {
    Lane_M4x4 r;
    r.e[0][0] = 1; r.e[0][1] = 0; r.e[0][2] = 0; r.e[0][3] = 0;
    r.e[1][0] = 0; r.e[1][1] = 1; r.e[1][2] = 0; r.e[1][3] = 0;
    r.e[2][0] = 0; r.e[2][1] = 0; r.e[2][2] = 1; r.e[2][3] = 0;
    r.e[3][0] = 0; r.e[3][1] = 0; r.e[3][2] = 0; r.e[3][3] = 1;

    return(r);
}

// TODO: Wide sinf implementation
Lane_F32 lane_sin(Lane_F32 angle) {
    Lane_F32 r;
    for(Int lane_i = 0; (lane_i < LANE_WIDTH); ++lane_i) {
        ((F32 *)&r)[lane_i] = sinf(extract(angle, lane_i));
    }

    return(r);
}

// TODO: Wide sinf implementation
Lane_F32 lane_cos(Lane_F32 angle) {
    Lane_F32 r;
    for(Int lane_i = 0; (lane_i < LANE_WIDTH); ++lane_i) {
        ((F32 *)&r)[lane_i] = cosf(extract(angle, lane_i));
    }

    return(r);
}

internal Lane_F32 crappy_radian_clamp(Lane_F32 a) {
    // TODO: Can't do a while loop like this for lanes.
    Lane_U32 less_than_0_mask      = (a < 0);
    Lane_U32 greater_than_2pi_mask = (a > LANE_PI32 * 2);

    Lane_F32 r = a;
    conditional_assign(less_than_0_mask,      &r, a + (LANE_PI32 * 2));
    conditional_assign(greater_than_2pi_mask, &r, a - (LANE_PI32 * 2));

    return(a);
}

internal Lane_M4x4 x_rotation(Lane_F32 angle) {
    angle = crappy_radian_clamp(angle);
    Lane_F32 s = lane_sin(angle);
    Lane_F32 c = lane_cos(angle);

    Lane_M4x4 r;
    r.e[0][0] = 1; r.e[0][1] = 0; r.e[0][2] =  0; r.e[0][3] = 0;
    r.e[1][0] = 0; r.e[1][1] = c; r.e[1][2] = -s; r.e[1][3] = 0;
    r.e[2][0] = 0; r.e[2][1] = s; r.e[2][2] =  c; r.e[2][3] = 0;
    r.e[3][0] = 0; r.e[3][1] = 0; r.e[3][2] =  0; r.e[3][3] = 1;

    return(r);
}

internal Lane_M4x4 y_rotation(Lane_F32 angle) {
    angle = crappy_radian_clamp(angle);
    Lane_F32 s = lane_sin(angle);
    Lane_F32 c = lane_cos(angle);

    Lane_M4x4 r;
    r.e[0][0] =  c; r.e[0][1] = 0; r.e[0][2] = s; r.e[0][3] = 0;
    r.e[1][0] =  0; r.e[1][1] = 1; r.e[1][2] = 0; r.e[1][3] = 0;
    r.e[2][0] = -s; r.e[2][1] = 0; r.e[2][2] = c; r.e[2][3] = 0;
    r.e[3][0] =  0; r.e[3][1] = 0; r.e[3][2] = 0; r.e[3][3] = 1;

    return(r);
}

internal Lane_M4x4 z_rotation(Lane_F32 angle) {
    angle = crappy_radian_clamp(angle);
    Lane_F32 s = lane_sin(angle);
    Lane_F32 c = lane_cos(angle);

    Lane_M4x4 r;
    r.e[0][0] = c; r.e[0][1] = -s; r.e[0][2] = 0; r.e[0][3] = 0;
    r.e[1][0] = s; r.e[1][1] =  c; r.e[1][2] = 0; r.e[1][3] = 0;
    r.e[2][0] = 0; r.e[2][1] =  0; r.e[2][2] = 1; r.e[2][3] = 0;
    r.e[3][0] = 0; r.e[3][1] =  0; r.e[3][2] = 0; r.e[3][3] = 1;

    return(r);
}

internal Lane_M4x4 transpose(Lane_M4x4 m) {
    Lane_M4x4 r;

    // TODO: Do this without loops.
    for(Int i = 0; (i < 4); ++i) {
        for(Int j = 0; (j < 4); ++j) {
            r.e[i][j] = m.e[j][i];
        }
    }

    return(r);
}

internal Lane_M4x4 columns3x3(Lane_V3 x, Lane_V3 y, Lane_V3 z) {
    Lane_M4x4 r;
    r.e[0][0] = x.x; r.e[0][1] = y.x; r.e[0][2] = z.x; r.e[0][3] = 0;
    r.e[1][0] = x.y; r.e[1][1] = y.y; r.e[1][2] = z.y; r.e[1][3] = 0;
    r.e[2][0] = x.z; r.e[2][1] = y.z; r.e[2][2] = z.z; r.e[2][3] = 0;
    r.e[3][0] = 0;   r.e[3][1] = 0;   r.e[3][2] = 0;   r.e[3][3] = 1;

    return(r);
}

internal Lane_M4x4 rows3x3(Lane_V3 x, Lane_V3 y, Lane_V3 z) {
    Lane_M4x4 r;
    r.e[0][0] = x.x; r.e[0][1] = x.y; r.e[0][2] = x.z; r.e[0][3] = 0;
    r.e[1][0] = y.x; r.e[1][1] = y.y; r.e[1][2] = y.z; r.e[1][3] = 0;
    r.e[2][0] = z.x; r.e[2][1] = z.y; r.e[2][2] = z.z; r.e[2][3] = 0;
    r.e[3][0] = 0;   r.e[3][1] = 0;   r.e[3][2] = 0;   r.e[3][3] = 1;

    return(r);
}

internal Lane_M4x4 translate(Lane_M4x4 a, Lane_V3 t) {
    a.e[0][3] += t.x;
    a.e[1][3] += t.y;
    a.e[2][3] += t.z;

    return(a);
}

internal Lane_M4x4 scale(Lane_M4x4 a, Lane_V3 s) {
    a.e[0][0] *= s.x;
    a.e[1][1] *= s.y;
    a.e[2][2] *= s.z;
    return(a);
}

internal Lane_V3 extract_scale(Lane_M4x4 a) {
    Lane_V3 r;
    r.x = a.e[0][0];
    r.y = a.e[1][1];
    r.z = a.e[2][2];

    return(r);
}

internal Lane_M4x4 camera_transform(Lane_V3 x, Lane_V3 y, Lane_V3 z, Lane_V3 p) {
    Lane_M4x4 r = rows3x3(x, y, z);
    r = translate(r, -(r * p));

    return(r);
}

internal void conditional_assign(Lane_U32 mask, Lane_M4x4 *dst, Lane_M4x4 src) {
    conditional_assign(mask, &dst->rows[0], src.rows[0]);
    conditional_assign(mask, &dst->rows[1], src.rows[1]);
    conditional_assign(mask, &dst->rows[2], src.rows[2]);
    conditional_assign(mask, &dst->rows[3], src.rows[3]);
}