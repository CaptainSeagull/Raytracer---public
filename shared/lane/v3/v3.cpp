LANE_PUBLIC_DEC V3 v3(V3 a) {
	V3 r;
	r.x = a.x;
	r.y = a.y;
	r.z = a.z;

	return(r);
}

LANE_PUBLIC_DEC V3 v3(float x, float y, float z) {
	V3 r;
	r.x = x;
	r.y = y;
	r.z = z;

	return(r);
}

LANE_PUBLIC_DEC V3 v3(float a) {
	V3 r;
	r.x = a;
	r.y = a;
	r.z = a;

	return(r);
}

LANE_PUBLIC_DEC V3 normalise_or_zero(V3 a) {
	V3 r = v3(0);

	float len_sq = length_sq(a);
	if(len_sq > float(0.0001f * 0.0001f)) {
		r = a * (float(1.0f) / square_root(len_sq));
	}

	return(r);
}

// Mul
LANE_PUBLIC_DEC V3 operator*(V3 a, float b) {
	V3 r;
	r.x = a.x * b;
	r.y = a.y * b;
	r.z = a.z * b;
	return(r);
}

LANE_PUBLIC_DEC V3 operator*(float a, V3 b) {
	V3 r = (b * a);
	return(r);
}

LANE_PUBLIC_DEC V3 operator*=(V3 &a, float b) {
	a = a * b;
	return(a);
}

// Div
LANE_PUBLIC_DEC V3 operator/(float a, V3 b) {
	V3 r;
	r.x = a / b.x;
	r.y = a / b.y;
	r.z = a / b.z;
	return(r);
}

LANE_PUBLIC_DEC V3 operator/(V3 a, float b) {
	V3 r;
	r.x = a.x / b;
	r.y = a.y / b;
	r.z = a.z / b;
	return(r);
}

LANE_PUBLIC_DEC V3 operator/=(V3 &a, float b) {
	a = a / b;
	return(a);
}

// Add
LANE_PUBLIC_DEC V3 operator+(V3 a, V3 b) {
	V3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;

	return(r);
}

LANE_PUBLIC_DEC V3 operator+=(V3 &a, V3 b) {
	a = a + b;
	return(a);
}

// Subtract
LANE_PUBLIC_DEC V3 operator-(V3 a, V3 b) {
	V3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;

	return(r);
}

LANE_PUBLIC_DEC V3 operator-(V3 a) {
	a = v3(0) - a;
	return(a);
}

LANE_PUBLIC_DEC V3 operator-=(V3 &a, V3 b) {
	a = a - b;
	return(a);
}

// And
LANE_PUBLIC_DEC V3 operator&(uint32_t a, V3 b) {
	V3 r;
	r.x = a & (uint32_t)b.x;
	r.y = a & (uint32_t)b.y;
	r.z = a & (uint32_t)b.z;

	return(r);
}

// Methods
LANE_PUBLIC_DEC float inner(V3 a, V3 b) {
	float r = ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
	return(r);
}

LANE_PUBLIC_DEC V3 hadamard(V3 a, V3 b) {
	V3 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;

	return(r);
}

LANE_PUBLIC_DEC V3 cross(V3 a, V3 b) {
	V3 r;
	r.x = ((a.y * b.z) - (a.z * b.y));
	r.y = ((a.z * b.x) - (a.x * b.z));
	r.z = ((a.x * b.y) - (a.y * b.x));

	return(r);
}

LANE_PUBLIC_DEC float length_sq(V3 a) {
	float r = inner(a, a);
	return(r);
}

LANE_PUBLIC_DEC float length(V3 a) {
	float r = square_root(length_sq(a)); // TODO: Use wide square root?
	return(r);
}

LANE_PUBLIC_DEC V3 normalise(V3 a) {
	V3 r = a * (1.0f / length(a));
	return(r);
}
