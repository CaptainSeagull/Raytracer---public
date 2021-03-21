OPTIMISED_FLAG=true
INTERNAL_FLAG=true
ALLOW_ASSERTS_FLAG=true
LANE_WIDTH=8

WARNINGS="-Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-parameter -Wno-writable-strings -Wno-unknown-escape-sequence -Wno-missing-field-initializers -Wno-missing-braces -Wno-char-subscripts -Wno-typedef-redefinition -Wno-deprecated-declarations"

INTERNAL=-DINTERNAL=1
if [ "$INTERNAL_FLAG" = "false" ]; then
    INTERNAL=-DINTERNAL=0
fi

ALLOW_ASSERTS=-DALLOW_ASSERTS=1
if [ "$ALLOW_ASSERTS_FLAG" = "false" ]; then
    ALLOW_ASSERTS=-DALLOW_ASSERTS=0
fi

LINKER="-ldl -lm -lrt -lX11 -lGL -lpthread"
FLAGS="-D_GNU_SOURCE $INTERNAL $ALLOW_ASSERTS -DLANE_WIDTH=$LANE_WIDTH $WARNINGS"

AVX_FLAG=""
if [ "$LANE_WIDTH" = "8" ]; then
    AVX_FLAG=-mavx2
else
    # TODO: _mm_floor_ps requires SSE4 not SSE2 on Clang? Seems odd...
    if [ "$LANE_WIDTH" = "4" ]; then
        AVX_FLAG=-msse4.1
    fi
fi

OPTIMISATION_LEVEL=""
if [ "$OPTIMISED_FLAG" = "true" ]; then
    OPTIMISATION_LEVEL=-O2
fi

echo "Building Raytracer"
clang -Wall -Wextra $OPTIMISATION_LEVEL src/build.cpp -std=c++11 -fno-exceptions $AVX_FLAG -fno-rtti -lm -o build/raytracer.out $FLAGS $WARNINGS -g $LINKER

pwd
