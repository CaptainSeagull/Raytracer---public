LANE_PUBLIC_DEC Lane_U32 xorshift32(Lane_U32 *entropy_series) {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    *entropy_series ^= *entropy_series << 13;
    *entropy_series ^= *entropy_series >> 17;
    *entropy_series ^= *entropy_series << 5;

    return(*entropy_series);
}

// TODO: Unit test that this is producing good distrobution of values (on input and output).
LANE_PUBLIC_DEC Lane_F32 random_unilateral(Lane_U32 *entropy_series) {
    // TODO: Why are we shifting this down again?
    Lane_F32 r = lane_f32_from_u32(xorshift32(entropy_series) >> 1) / (float)(LANE_U32_MAX >> 1);
    return(r);
}

LANE_PUBLIC_DEC Lane_F32 random_bilateral(Lane_U32 *entropy_series) {
    Lane_F32 r = -1.0f + (2.0f * random_unilateral(entropy_series));
    return(r);
}

LANE_PUBLIC_DEC uint32_t xorshift32(uint32_t *entropy_series) {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    *entropy_series ^= *entropy_series << 13;
    *entropy_series ^= *entropy_series >> 17;
    *entropy_series ^= *entropy_series << 5;

    return(*entropy_series);
}

// TODO: Unit test that this is producing good distrobution of values (on input and output).
LANE_PUBLIC_DEC float random_unilateral(uint32_t *entropy_series) {
    // TODO: Why are we shifting this down again?
    float r = (float)(xorshift32(entropy_series) >> 1) / (float)(LANE_U32_MAX >> 1);
    return(r);
}

LANE_PUBLIC_DEC float random_bilateral(uint32_t *entropy_series) {
    float r = -1.0f + (2.0f * random_unilateral(entropy_series));
    return(r);
}
