/*  string.h - v0.1 - public domain helper library - no warranty implied; use at your own risk

    Utility for working with strings in C++.
    Note that this class does NO ALLOCATIONS. It expects calling code to deal with mallocing/freeing of the memory.
    Makes sense in my projects... your milage may vary...

    Write #define STRING_IMPLEMENTATION in ONE of the C/C++ files to create the implementation.

    // It should look like this.
    #define STRING_IMPLEMENTATION
    #include "string.h"

    // Some defines to change behaviour of the file. MUST be in the same file as MEMORY_ARENA_IMPLEMENTATION.
    #define STRING_ASSERT(expression) - Override the default ASSERT used.
    #define STRING_ALLOW_ASSERT       - Whether to turn ASSERTS on/off. Has no effect if STRING_ASSERT is defined.
    #define STRING_PUBLIC_DEC         - Allows functions to have a declaration. Can use static or inline if required.
    #define STRING_SIZE_TYPE          - Change the type of the size member for the struct. By default it's an int.
    #define STRING_CONST_STRING       - Define this to change the type in struct String from "char *" to "char const *" (if that's your thing).

    Usage:

    #define STRING_IMPLEMENTATION
    #include "string.h"
    int main(int argc, char **argv) {
        Cpp_String::String my_string = "Hello, World!"; // operator= is overloaded
        int length = my_string.len;

        Char as_cstring[1024] = {};
        memcpy(as_cstring, my_string.e, my_string.len); // Assumes my_string.len < 1024

        Cpp_String::String my_string2 = "2.44);"
        Cpp_String::String_To_Float_Result r = string_to_float(my_string2);
        if(r.success) {
            float as_float = r.v;
        }

        Cpp_String::String a = "Test";
        Cpp_String::String b = "Test";
        if(a == b) { // operator== is overloaded.
            // They're the same!
        }

        return(0);
    }

    LICENSE at end of file.
*/

#if !defined(_STRING_H_INCLUDE)
#define _STRING_H_INCLUDE

#if !defined(STRING_ASSERT)
    #if defined(MEMORY_ARENA_ALLOW_ASSERT)
        #define STRING_ASSERT(exp) do { static int ignore = 0; if(!ignore) { if(!(exp)) {*(int volatile *)0 = 0; } } } while(0)
    #else
        #define STRING_ASSERT(exp) {}
    #endif
#endif

#if !defined(STRING_SIZE_TYPE)
    #define STRING_SIZE_TYPE int
#endif

#if !defined(STRING_PUBLIC_DEC)
    #define STRING_PUBLIC_DEC
#endif

#define STRING_CONST_MODIFIER /*empty*/
#if defined(STRING_CONST_STRING)
    #undef STRING_CONST_MODIFIER
    #define STRING_CONST_MODIFIER const
#endif

struct String {
    // TODO: Having "= 0" in here means we can't actually create an uninitialised string anymore... maybe better to just get rid
    //       of constructors?
    char STRING_CONST_MODIFIER *e = 0;
    STRING_SIZE_TYPE len = 0;

    inline String() {}
    inline String(char *c_string);
    inline String(char const *c_string);
};

struct String_To_Int_Result {
    int/*bool*/ success;
    int v;
};

struct String_To_Float_Result {
    int/*bool*/ success;
    float v;
};

STRING_PUBLIC_DEC String create_string(char *str, STRING_SIZE_TYPE len = 0);
STRING_PUBLIC_DEC String create_string(char const *str, STRING_SIZE_TYPE len = 0);
STRING_PUBLIC_DEC String create_substring(String str, STRING_SIZE_TYPE start, STRING_SIZE_TYPE end = 1);
STRING_PUBLIC_DEC int/*bool*/ string_compare(String a, String b);
STRING_PUBLIC_DEC int/*bool*/ operator==(String a, String b);
STRING_PUBLIC_DEC int/*bool*/ string_contains(String a, String b);

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_length(char *str);
STRING_PUBLIC_DEC STRING_SIZE_TYPE string_length(char const *str);

STRING_PUBLIC_DEC int char_to_int(char c);
STRING_PUBLIC_DEC String_To_Int_Result string_to_int(String s);
STRING_PUBLIC_DEC String_To_Float_Result string_to_float(String s);

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char *dst, char *src);
STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char const *dst, char const *src);
STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char *dst, char const *src);
STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char *dst, char *src);

STRING_PUBLIC_DEC int/*bool*/ string_contains(String str, char target);


#if defined(STRING_IMPLEMENTATION)

String::String(char *c_string) {
    String s = create_string(c_string);
    this->e = s.e;
    this->len = s.len;
}

String::String(char const *c_string) {
    String s = create_string((char *)c_string);
    this->e = s.e;
    this->len = s.len;
}

STRING_PUBLIC_DEC String create_string(char *str, STRING_SIZE_TYPE len/*= 0*/) {
    STRING_ASSERT(str);

    String res;
    res.e = str;
    res.len = (len) ? len : string_length(str);

    return(res);
}

STRING_PUBLIC_DEC String create_string(char const *str, STRING_SIZE_TYPE len/*= 0*/) {
    String res = create_string((char *)str, len);

    return(res);
}

STRING_PUBLIC_DEC String create_substring(String str, STRING_SIZE_TYPE start, STRING_SIZE_TYPE end/*= 1*/) {
    if(end == -1) { end = str.len; }

    String res = create_string(str.e + start, end - start);

    return(res);
}

STRING_PUBLIC_DEC int/*bool*/ string_compare(String a, String b) {
    int/*bool*/ res = false;
    if(a.len == b.len) {
        res = true;

        for(STRING_SIZE_TYPE i = 0; (i < a.len); ++i) {
            if(a.e[i] != b.e[i]) {
                res = false;
                break; // while
            }
        }
    }

    return(res);
}

STRING_PUBLIC_DEC int/*bool*/ operator==(String a, String b) {
    int/*bool*/ r = string_compare(a, b);
    return(r);
}

STRING_PUBLIC_DEC int/*bool*/ string_contains(String a, String b) {
    int/*bool*/ res = false;
    for(STRING_SIZE_TYPE i = 0; (i < a.len); ++i) {
        // TODO: Doesn't work for unisgned numbers. Also... why the hell am I doing b.len < 0??
        if((a.len - i) - b.len < 0) {
            break; // for
        } else {
            String a_tmp = create_string(&a.e[i], b.len);
            if(string_compare(a_tmp, b)) {
                res = true;
                break; // for
            }
        }
    }

    return(res);
}

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_length(char *str) {
    STRING_ASSERT(str);

    STRING_SIZE_TYPE res = 0;
    while(*str++) { ++res; }

    return(res);
}

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_length(char const *str) {
    return string_length((char *)str);
}

STRING_PUBLIC_DEC int char_to_int(char c) {
    int r = -1;
    switch(c) {
        case '0': { r = 0; } break;
        case '1': { r = 1; } break;
        case '2': { r = 2; } break;
        case '3': { r = 3; } break;
        case '4': { r = 4; } break;
        case '5': { r = 5; } break;
        case '6': { r = 6; } break;
        case '7': { r = 7; } break;
        case '8': { r = 8; } break;
        case '9': { r = 9; } break;
    }

    return(r);
}

STRING_PUBLIC_DEC String_To_Int_Result string_to_int(String s) {
    int a = 0;
    int/*bool*/ success = false;
    int/*bool*/ is_negative = (s.e[0] == '-') ? true : false;

    for(STRING_SIZE_TYPE i = (is_negative) ? 1 : 0; (i < s.len); ++i) {
        int t = char_to_int(s.e[i]);
        if(t == -1) {
            break; // for - something went wrong
        } else {
            a *= 10;
            a += t;

            if(i == (s.len - 1)) {
                success = true;
            }
        }
    }

    String_To_Int_Result r = {};
    if(success) {
        r.v = (is_negative) ? -a : a;
        r.success = true;
    }

    return(r);
}

STRING_PUBLIC_DEC String_To_Float_Result string_to_float(String s) {
    // a is before decimal point, b is after.
    float a = 0.0f, b = 0.0f;
    int/*Bool*/ a_success = false, b_success = false;
    int/*Bool*/ is_negative = (s.e[0] == '-') ? true : false;

    // TODO: Maybe we can do this by only having one for loop. We keep track of where the decimal point is
    //       relative to the end of the string, and just divide the result by that?

    for(STRING_SIZE_TYPE i = (is_negative) ? 1 : 0; (i < s.len); ++i) {
        if(s.e[i] == '.') {
            a_success = true;
            break;
        } else {
            int t = char_to_int(s.e[i]);
            if(t == -1) {
                break; // for - something went wrong
            } else {
                a *= 10.0f;
                a += (float)t;

                if(i == (s.len - 1)) {
                    // String had no decimal point
                    a_success = true;
                    b_success = true;
                }
            }
        }
    }

    if(!b_success) { // Only called if string has a decimal point
        for(STRING_SIZE_TYPE i = s.len - 1; (i >= 0); --i) {
            if(s.e[i] == '.') {
                b_success = true;
                break;
            } else {
                int t = char_to_int(s.e[i]);
                if(t == -1) {
                    break; // for - something went wrong
                } else {
                    b /= 10.0f;
                    b -= 0.1f * (float)t;
                }
            }
        }
    }

    String_To_Float_Result r = {};
    if(a_success && b_success) {
        // We calculate b as the negative of itself, so invert it before adding the result together.
        r.v = a + -b;
        if(is_negative) { r.v = -r.v; }
        r.success = true;
    }

    return(r);
}

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char *dst, char *src) {
    STRING_SIZE_TYPE res = 0;
    while(*src) {
        *dst = *src;
        ++dst;
        ++src;
        ++res; // TODO: Could calculate at end, rather than increment every time through the loop?
    }

    return(res);
}

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char const *dst, char const *src) {
    STRING_SIZE_TYPE r = string_copy((char *)dst, (char *)src);
    return(r);
}

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char *dst, char const *src) {
    STRING_SIZE_TYPE r = string_copy(dst, (char *)src);
    return(r);
}

STRING_PUBLIC_DEC STRING_SIZE_TYPE string_copy(char const *dst, char *src) {
    STRING_SIZE_TYPE r = string_copy((char *)dst, src);
    return(r);
}

STRING_PUBLIC_DEC int/*bool*/ string_contains(String str, char target) {
    int/*bool*/ res = false;

    for(int i = 0; (i < str.len); ++i) {
        if(str.e[i] == target) {
            res = true;
            break;
        }
    }

    return(res);
}


#endif // STRING_IMPLEMENTATION

#endif // _STRING_H_INCLUDE

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
