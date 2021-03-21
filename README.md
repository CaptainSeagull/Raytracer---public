# Raytracer


![Raytracer scene](https://github.com/CaptainSeagull/Raytracer/blob/dev/data/readme/cover.png)

## Use the files in prebuilt to run this without building
(These are out-of-date with current build)

prebuilt/avx2:   avx2 version of the raytracer.

prebuilt/sse2:   sse2 version of the raytracer

prebuilt/scalar: scalar version of the raytracer (probably still uses sse2 instructions, will just be 4x slower...)


## Controls
Arrow keys - rotate camera

WASD - move camera

spacebar (hold) - increment the rays-per-pixel for the scene, increasing quality

J - save the current scene to disk

Keys 0 - 4 - Change the scene being rendered.

Left-click  - Pan

Right-Click - Rotate camera (NOT orbit)

## Building
To build install Visual Studio, start a command shell, and run "vcvarsall.bat x64" to setup the paths and environment variables. Then run win32_msvc.bat to build the program. This will create a build directory with raytracer.exe which can be run.

In order for it to run point the Working Directory at the data directory. If you don't care about scene 0 should work without that though.

Some flags which can be edited in win32_msvc.bat.

OPTIMISED_FLAG - O2 or OD build, basically

INTERNAL_FLAG - Some internal reporting like a text overley.

ALLOW_ASSERTS - Enable/disable asserts

LANE_WIDTH - Lane width to compile for. Supports 1, 4, and 8.


## Directory structure
### src
Contains all the source files used by the program.

build.cpp - the only C++ file built by the compiler (it #includes all the others)

image.cpp - barebones Bitmap loader

m4x4.cpp - barebones 4x4 matrix implementation

main.cpp - the main loop for the raytracer

platform.h - common header which contains an interface to all OS-specific code

platform_win32.cpp - Windows implementation of platform code

raytracer.cpp - main raytracer code. Defines interface / implementation

scene_builder.cpp - barebones JSON loader used to build scenes

utils.h - common utils


### shared
lane - SIMD helper library

defer.h - defer in C++ implementation

memory_arena.h - utility to use memory arenas in C++

stb_sprintf.h - fast sprintf implementation

string.h - C++ string class

token.h - simple tokenizer/lexer


### data
test.json - the JSON file exported from SketchUp. Only contains one face right now.

output - directory containing anything the raytracer decided to output.
