OPTIMISED=false
INTERNAL=true
ALLOW_ASSERTS=true

# Mirror
WARNINGS="-Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-parameter -Wno-writable-strings -Wno-unknown-escape-sequence -Wno-missing-field-initializers -Wno-missing-braces -Wno-char-subscripts -Wno-typedef-redefinition"  

# Internal
if [ "$INTERNAL" = "true" ]; then
    INTERNAL=-DINTERNAL=1
else
    INTERNAL=-DINTERNAL=0
fi

# Allow asserts
if [ "$ALLOW_ASSERTS" = "true" ]; then
    ALLOW_ASSERTS=-DALLOW_ASSERTS=1
else
    ALLOW_ASSERTS=-DALLOW_ASSERTS=0
fi

# TODO: Only need -mavx2 flag if LANE_WIDTH=8

echo "Building Raytracer"
clang -Wall -Wextra src/build.cpp -std=c++11 -fno-exceptions -mavx2 -fno-rtti -lm -o build/raytracer.so -DUSE_SDL=1 $INTERNAL $ALLOW_ASSERTS -DLANE_WIDTH=8 -DINTERNAL=1 $WARNINGS -g -ldl -lm -lrt `sdl2-config --libs --cflags`

pwd
