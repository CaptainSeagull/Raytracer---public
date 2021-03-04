LANE_PUBLIC_DEC V4 v4(V4 a) {
	V4 r;
	r.x = a.x;
	r.y = a.y;
	r.z = a.z;
	r.w = a.w;

	return(r);
}

LANE_PUBLIC_DEC V4 v4(float x, float y, float z, float w) {
	V4 r;
	r.x = x;
	r.y = y;
	r.z = z;
	r.w = w;

	return(r);
}

LANE_PUBLIC_DEC V4 v4(float a) {
	V4 r;
	r.x = a;
	r.y = a;
	r.z = a;
	r.w = a;

	return(r);
}

LANE_PUBLIC_DEC V4 normalise_or_zero(V4 a) {
	V4 r = {};

	float len_sq = length_sq(a);
	if(len_sq > (0.0001f * 0.0001f)) {
		r = a * (1.0f / square_root(len_sq));
	}

	return(r);
}

// Mul
LANE_PUBLIC_DEC V4 operator*(V4 a, float b) {
	V4 r;
	r.x = a.x * b;
	r.y = a.y * b;
	r.z = a.z * b;
	r.w = a.w * b;

	return(r);
}

LANE_PUBLIC_DEC V4 operator*(float a, V4 b) {
	V4 r = (b * a);
	return(r);
}

LANE_PUBLIC_DEC V4 operator*=(V4 &a, float b) {
	a = a * b;
	return(a);
}

// Div
LANE_PUBLIC_DEC V4 operator/(float a, V4 b) {
	V4 r;
	r.x = a / b.x;
	r.y = a / b.y;
	r.z = a / b.z;
	r.w = a / b.w;
	return(r);
}

LANE_PUBLIC_DEC V4 operator/(V4 a, float b) {
	V4 r;
	r.x = a.x / b;
	r.y = a.y / b;
	r.z = a.z / b;
	r.w = a.w / b;
	return(r);
}

LANE_PUBLIC_DEC V4 operator/=(V4 &a, float b) {
	a = a / b;
	return(a);
}

// Add
LANE_PUBLIC_DEC V4 operator+(V4 a, V4 b) {
	V4 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	r.w = a.w + b.w;

	return(r);
}

LANE_PUBLIC_DEC V4 operator+=(V4 &a, V4 b) {
	a = a + b;
	return(a);
}

// Subtract
LANE_PUBLIC_DEC V4 operator-(V4 a, V4 b) {
	V4 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	r.w = a.w - b.w;

	return(r);
}

LANE_PUBLIC_DEC V4 operator-(V4 a) {
	a = v4(0) - a;
	return(a);
}

LANE_PUBLIC_DEC V4 operator-=(V4 &a, V4 b) {
	a = a - b;
	return(a);
}

// And
LANE_PUBLIC_DEC V4 operator&(uint32_t a, V4 b) {
	V4 r;
	r.x = a & (uint32_t)b.x;
	r.y = a & (uint32_t)b.y;
	r.z = a & (uint32_t)b.z;
	r.w = a & (uint32_t)b.w;

	return(r);
}

// Methods
LANE_PUBLIC_DEC float inner(V4 a, V4 b) {
	float r = ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
	return(r);
}

LANE_PUBLIC_DEC V4 hadamard(V4 a, V4 b) {
	V4 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
	r.w = a.w * b.w;

	return(r);
}

LANE_PUBLIC_DEC float length_sq(V4 a) {
	float r = inner(a, a);
	return(r);
}

LANE_PUBLIC_DEC float length(V4 a) {
	float r = square_root(length_sq(a)); // TODO: Use wide square root?
	return(r);
}

LANE_PUBLIC_DEC V4 normalise(V4 a) {
	V4 r = a * (1.0f / length(a));
	return(r);
}
