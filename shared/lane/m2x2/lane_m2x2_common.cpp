LANE_PUBLIC_DEC Lane_M2x2 lane_outer(V2 a, V2 b) {
	Lane_M2x2 r;
	r.a = a.x * b.x;
	r.b = a.x * b.y;
	r.c = a.y * b.x;
	r.d = a.y * b.y;

	return(r);
}