/*  defer.h - v0.1 - public domain helper library - no warranty implied; use at your own risk

    C++ implementation of Go-like defer

    NOTE that this will probably cause errors if includes in multiple files... so if you're not using unity built systems... good luck! :-)

    Use #define DEFER_HIDE_SHORTNAME to hide the "defer" shorthand name. In that case use _DEFER_CPP instead.

    Usage:

    int main(int argc, char **argv) {
        {
            void *p = malloc(sizeof(p));
            defer { free(p); };

            ...

            // We free p here, at scope end
        }

        {
            printf("1");
            defer { printf("2"); };
            printf("3");

            // Prints 1, 3, 2
        }

        return(0);
    }

    LICENSE at end of file.
*/

#if !defined(_defer_hpp)

template<typename F>
struct internal_Defer_Struct {
    F f;
    inline internal_Defer_Struct(F f) : f(f) {}
    inline ~internal_Defer_Struct() { f(); }
    internal_Defer_Struct<F> operator=(internal_Defer_Struct<F> other) {
        // Visual Studio was complaining about this... but it shouldn't ever be called...
        //(*int *)0 = 0; // Force an interupt if this is called... Clang doesn't like the interupt...
        internal_Defer_Struct<F> r;
        return(r);
    }
};

struct {
    template<typename F>
    inline internal_Defer_Struct<F> operator<<(F f) {
        return internal_Defer_Struct<F>(f);
    }
} internal_Defer_Functor;

#define INTERNAL_DEFER_CONCAT_(a, b) a##b
#define INTERNAL_DEFER_CONCAT(a, b) INTERNAL_DEFER_CONCAT_(a, b)
#define _DEFER_CPP auto INTERNAL_DEFER_CONCAT(defer_in_cpp_, __COUNTER__) = internal_Defer_Functor << [&]

#if !defined(DEFER_HIDE_SHORTNAME)
    #define defer _DEFER_CPP
#endif

#define _defer_hpp
#endif


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2021 Jonathan Livingstone
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
