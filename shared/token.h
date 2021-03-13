/*  token.h - v0.1 - public domain helper library - no warranty implied; use at your own risk

    Utility for tokenising a string with tokens separated by spaces.

    Write #define TOKEN_IMPLEMENTATION in ONE of the C/C++ files to create the implementation.

    // It should look like this.
    #define TOKEN_IMPLEMENTATION
    #include "token.h"

    // Some defines that can be overridden. Must be defined in same file as TOKEN_IMPLEMENTATION
    #define TOKEN_ASSERT(expression)    - Override the default ASSERT used.
    #define TOKEN_ALLOW_ASSERT          - Whether to turn ASSERTS on/off. Has no effect if STRING_ASSERT is defined.
    #define TOKEN_PUBLIC_DEC            - Allows functions to have a declaration. Can use static or inline if required.

    #define TOKEN_IS_END_OF_LINE(c)     - What we consider the end of a line.
    #define TOKEN_IS_WHITESPACE(c)      - What we consider whitespace.
    #define TOKEN_IS_ALPHABETICAL(c)    - What we consider alphabetical characters.
    #define TOKEN_IS_NUMBER(c)          - What we consider numeric characters.
    #define TOKEN_IS_HEXIDECIMAL        - What we consider hexidecimal (0-9, a-f).

    #define TOKEN_SKIP_CPP_COMMENTS     - Treat C++-style comments as whitespace.
    #define TOKEN_TOKEN_SKIP_C_COMMENTS - Treat C-style comments as whitespace.

    Usage:

    #define TOKEN_IMPLEMENTATION
    #include "token.h"

    int main(int argc, char **argv) {
        char const *stream = "int main(int argc, char **argv) { return(0); }"

        Tokenizer_Cpp::Tokenizer tokenizer = create_tokenizer(stream);

        // Calling get_token in a loop like this will return the following as tokens:
        //   -> "int" "main" "(" "int" "argc" "," "char" "*" "*" "argv" ")" "{" "return" "(" "0" ")" ";" "}"
        // Note that tokens ARE NOT null terminated... so need to copy with length if using. It just stores a pointer to where in "stream"
        // the start is, and how many bytes to read. Need to take into account when using.
        Token token = Tokenizer_Cpp::get_token(&tokenizer);
        do {
            switch(token.type) {
                case Tokenizer_Cpp::Token_Type_identifier {
                    // Do identifier stuff
                } break;

                case Tokenizer_Cpp::Token_Type_open_paren {
                    // Do open paren stuff
                } break;

                // Handle as many as you like!
            }

            token = Tokenizer_Cpp::get_token(&tokenizer);
        } while(token.type != Tokenizer_Cpp::Token_Type_end_of_stream);

        return(0);
    }

    LICENSE at end of file.
*/

#if !defined(_TOKEN_H)
#define _TOKEN_H

//
// Types
#if !defined(TOKEN_PUBLIC_DEC)
    #define TOKEN_PUBLIC_DEC
#endif

namespace Tokenizer_Cpp {

enum Token_Type {
    Token_Type_unknown,

    Token_Type_open_paren,
    Token_Type_close_paren,
    Token_Type_colon,
    Token_Type_double_colon,
    Token_Type_semi_colon,
    Token_Type_asterisk,
    Token_Type_open_bracket,
    Token_Type_close_bracket,
    Token_Type_open_brace,
    Token_Type_close_brace,
    Token_Type_open_angle_bracket,
    Token_Type_close_angle_bracket,
    Token_Type_hash,
    Token_Type_double_hash,
    Token_Type_assign,
    Token_Type_dereference,
    Token_Type_comma,
    Token_Type_tilde,
    Token_Type_period,
    Token_Type_ampersand,
    Token_Type_inclusive_or,
    Token_Type_exclusive_or,
    Token_Type_not,

    Token_Type_plus,
    Token_Type_minus,
    Token_Type_divide,
    Token_Type_modulo,

    Token_Type_increment,
    Token_Type_decrement,

    Token_Type_plus_equal,
    Token_Type_minus_equal,
    Token_Type_times_equal,
    Token_Type_divide_equal,

    Token_Type_equal,
    Token_Type_not_equal,
    Token_Type_greater_than_or_equal,
    Token_Type_less_than_or_equal,

    Token_Type_number, // TODO: Split out ints and floats?
    Token_Type_hex_number,
    Token_Type_identifier,
    Token_Type_string,
    Token_Type_char,
    Token_Type_var_args,

    Token_Type_left_shift,
    Token_Type_right_shift,

    Token_Type_and,
    Token_Type_or,

    Token_ternary,

    Token_Type_backslash,

    Token_Type_error,

    Token_Type_end_of_stream,

    Token_Type_count
};

struct Token {
    char const *e;
    int unsigned len;
    Token_Type type;
    int row, col;
};

struct Tokenizer {
    char const *at;
    // TODO: Add a length in here as well.
    int row, col;
};

TOKEN_PUBLIC_DEC Tokenizer create_tokenizer(char const *input_stream);
TOKEN_PUBLIC_DEC Token peak_token(Tokenizer *tokenizer);

TOKEN_PUBLIC_DEC void eat_token(Tokenizer *tokenizer);
TOKEN_PUBLIC_DEC void eat_tokens(Tokenizer *tokenizer, int num_tokens_to_eat);

TOKEN_PUBLIC_DEC Token get_token(Tokenizer *tokenizer);

TOKEN_PUBLIC_DEC char const *token_type_to_string(Token_Type type);

//
// Source file
//
#if defined(TOKEN_IMPLEMENTATION)

#if !defined(TOKEN_ASSERT)
    #if defined(TOKEN_ALLOW_ASSERT)
        #define TOKEN_ASSERT(exp) do { static int ignore = 0; if(!ignore) { if(!(exp)) {*(int volatile *)0 = 0; } } } while(0)
    #else
        #define TOKEN_ASSERT(exp) {}
    #endif
#endif

//
// Overrideable macros
#if !defined(TOKEN_IS_END_OF_LINE)
    #define TOKEN_IS_END_OF_LINE(c) ((c == '\n') || (c == '\r'))
#endif

#if !defined(TOKEN_IS_WHITESPACE)
    #define TOKEN_IS_WHITESPACE(c) ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f') || (TOKEN_IS_END_OF_LINE(c)))
#endif

#if !defined(TOKEN_IS_ALPHABETICAL)
    #define TOKEN_IS_ALPHABETICAL(c) (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
#endif

#if !defined(TOKEN_IS_NUMBER)
    #define TOKEN_IS_NUMBER(c) ((c >= '0') && (c <= '9'))
#endif

#if !defined(TOKEN_IS_HEXIDECIMAL)
    #define TOKEN_IS_HEXIDECIMAL(c) (TOKEN_IS_NUMBER(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
#endif

static void internal_tokenizer_advance(Tokenizer *tokenizer, int size_to_advance) {
    for(int i = 0; (i < size_to_advance); ++i) {
        ++tokenizer->at;
        ++tokenizer->col;
        if(TOKEN_IS_END_OF_LINE(tokenizer->at[0])) {
            ++tokenizer->row;
            tokenizer->col = 0;
        }
    }
}

static void internal_eat_whitespace(Tokenizer *tokenizer) {
    for(;;) {
        if(!tokenizer->at[0]) { // Nul terminator.
            break; // for
        } else if(TOKEN_IS_WHITESPACE(tokenizer->at[0])) { // Whitespace
            internal_tokenizer_advance(tokenizer, 1);
#if defined(TOKEN_SKIP_CPP_COMMENTS)
        } else if((tokenizer->at[0] == '/') && (tokenizer->at[1] == '/')) { // C++ comments.
            internal_tokenizer_advance(tokenizer, 2);
            while((tokenizer->at[0]) && (!TOKEN_IS_END_OF_LINE(tokenizer->at[0]))) {
                internal_tokenizer_advance(tokenizer, 1);
            }
#endif // SKIP_CPP_COMMENTS
#if defined(TOKEN_TOKEN_SKIP_C_COMMENTS)
        } else if((tokenizer->at[0] == '/') && (tokenizer->at[1] == '*')) { // C comments.
            internal_tokenizer_advance(tokenizer, 2);
            while((tokenizer->at[0]) && !((tokenizer->at[0] == '*') && (tokenizer->at[1] == '/'))) {
                internal_tokenizer_advance(tokenizer, 1);
            }
            if(tokenizer->at[0] == '*') {
                internal_tokenizer_advance(tokenizer, 2); // TODO: What is this doing?
            }
#endif // SKIP_C_COMMENTS
        } else {
            break; // for
        }
    }
}

//
// Public methods
TOKEN_PUBLIC_DEC Tokenizer create_tokenizer(char const *input_stream) {
    Tokenizer tokenizer = {};
    tokenizer.at = input_stream;

    return(tokenizer);
}

TOKEN_PUBLIC_DEC Token peak_token(Tokenizer *tokenizer) {
    Tokenizer copy_tokenizer = *tokenizer;
    Token res = get_token(&copy_tokenizer);

    return(res);
}

TOKEN_PUBLIC_DEC void eat_token(Tokenizer *tokenizer) {
    eat_tokens(tokenizer, 1);
}

TOKEN_PUBLIC_DEC void eat_tokens(Tokenizer *tokenizer, int num_tokens_to_eat) {
    for(int i = 0; (i < num_tokens_to_eat); ++i) {
        get_token(tokenizer);
    }
}

TOKEN_PUBLIC_DEC Token get_token(Tokenizer *tokenizer) {
    Token res = {};
    internal_eat_whitespace(tokenizer);

    if(tokenizer->at[0] == 0) {
        res.type = Token_Type_end_of_stream;
    } else {
        res.len = 1;
        res.e = tokenizer->at;
        res.row = tokenizer->row;
        res.col = tokenizer->col;
        char c = tokenizer->at[0];
        internal_tokenizer_advance(tokenizer, 1);

        switch(c) {
            case '(':  { res.type = Token_Type_open_paren;    } break;
            case ')':  { res.type = Token_Type_close_paren;   } break;
            case ';':  { res.type = Token_Type_semi_colon;    } break;
            case '[':  { res.type = Token_Type_open_bracket;  } break;
            case ']':  { res.type = Token_Type_close_bracket; } break;
            case '{':  { res.type = Token_Type_open_brace;    } break;
            case '}':  { res.type = Token_Type_close_brace;   } break;
            case ',':  { res.type = Token_Type_comma;         } break;
            case '~':  { res.type = Token_Type_tilde;         } break;
            case '%':  { res.type = Token_Type_modulo;        } break;
            case '?':  { res.type = Token_ternary;            } break;
            case '^':  { res.type = Token_Type_exclusive_or;  } break;
            case '\\': { res.type = Token_Type_backslash;     } break;

            case '#': {
                if(tokenizer->at[0] == '#') { res.type = Token_Type_double_hash; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                        { res.type = Token_Type_hash; }
            } break;

            case ':': {
                if(tokenizer->at[0] == ':') { res.type = Token_Type_double_colon; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                        { res.type = Token_Type_colon; }
            } break;

            case '&': {
                if(tokenizer->at[0] == '&') { res.type = Token_Type_and; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                        { res.type = Token_Type_ampersand; }
            } break;

            case '|': {
                if(tokenizer->at[0] == '|') { res.type = Token_Type_or; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                        { res.type = Token_Type_inclusive_or; }
            } break;

            case '=': {
                if(tokenizer->at[0] == '=')      { res.type = Token_Type_equal;  res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else if(tokenizer->at[0] == '>') { res.type = Token_Type_dereference;  res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                             { res.type = Token_Type_assign; }
            } break;

            case '!': {
                if(tokenizer->at[0] == '=') { res.type = Token_Type_not_equal; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                        { res.type = Token_Type_not; }
            } break;

            case '>': {
                if(tokenizer->at[0] == '=')      { res.type = Token_Type_greater_than_or_equal; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else if(tokenizer->at[0] == '>') { res.type = Token_Type_right_shift; res.len = 2; internal_tokenizer_advance(tokenizer, 1);    }
                else                             { res.type = Token_Type_close_angle_bracket; }
            } break;

            case '+': {
                if(tokenizer->at[0] == '+')      { res.type = Token_Type_increment;  res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else if(tokenizer->at[0] == '=') { res.type = Token_Type_plus_equal; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                             { res.type = Token_Type_plus; }
            } break;

            case '-': {
                if(tokenizer->at[0] == '-')      { res.type = Token_Type_decrement; res.len = 2; internal_tokenizer_advance(tokenizer, 1);   }
                else if(tokenizer->at[0] == '=') { res.type = Token_Type_minus_equal; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                             { res.type = Token_Type_minus; }
            } break;

            case '*': {
                if(tokenizer->at[0] == '=') { res.type = Token_Type_times_equal; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                        { res.type = Token_Type_asterisk; }
            } break;

            case '/': {
                if(tokenizer->at[0] == '=') { res.type = Token_Type_times_equal; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else                        { res.type = Token_Type_divide; }
            } break;

            case '<': {
                if(tokenizer->at[0] == '=')      { res.type = Token_Type_less_than_or_equal; res.len = 2; internal_tokenizer_advance(tokenizer, 1); }
                else if(tokenizer->at[0] == '<') { res.type = Token_Type_left_shift; res.len = 2; internal_tokenizer_advance(tokenizer, 1);    }
                else                             { res.type = Token_Type_open_angle_bracket; }
            } break;

            case '.':  {
                int/*bool*/ var_args = 0;/*false*/
                if((tokenizer->at[0] == '.') && (tokenizer->at[1] == '.')) {
                    var_args = 1;/*true*/

                    res.type = Token_Type_var_args;
                    res.len = 3;

                    internal_tokenizer_advance(tokenizer, 2);
                }

                if(!var_args) {
                    res.type = Token_Type_period;
                }
            } break;

            case '"': {
                res.e = tokenizer->at;
                while((tokenizer->at[0]) && (tokenizer->at[0] != '"')) {
                    if((tokenizer->at[0] == '\\') && (tokenizer->at[1])) { internal_tokenizer_advance(tokenizer, 1); }
                    internal_tokenizer_advance(tokenizer, 1);
                }

                res.type = Token_Type_string;
                res.len = (tokenizer->at - res.e);
                if(tokenizer->at[0] == '"') {
                    internal_tokenizer_advance(tokenizer, 1);
                }
            } break;

            case '\'': {
                res.e = tokenizer->at;
                while((tokenizer->at[0]) && (tokenizer->at[0] != '\'')) {
                    if((tokenizer->at[0] == '\\') && (tokenizer->at[1])) { internal_tokenizer_advance(tokenizer, 1); }
                    internal_tokenizer_advance(tokenizer, 1);
                }

                res.type = Token_Type_char;
                res.len = (tokenizer->at - res.e);
                if(tokenizer->at[0] == '\'') {
                    internal_tokenizer_advance(tokenizer, 1);
                }
            } break;

            default: {
                if((TOKEN_IS_ALPHABETICAL(c)) || (c == '_')) {
                    while((TOKEN_IS_ALPHABETICAL(tokenizer->at[0])) || (TOKEN_IS_NUMBER(tokenizer->at[0])) || (tokenizer->at[0] == '_')) {
                        internal_tokenizer_advance(tokenizer, 1);
                    }

                    res.len = (tokenizer->at - res.e);
                    res.type = Token_Type_identifier;
                } else if(TOKEN_IS_NUMBER(c)) {
                    if((c == '0' && tokenizer->at[0] == 'x') || tokenizer->at[0] == 'X') {
                        internal_tokenizer_advance(tokenizer, 1);
                        while(TOKEN_IS_HEXIDECIMAL(tokenizer->at[0])) {
                            internal_tokenizer_advance(tokenizer, 1);
                        }

                        res.type = Token_Type_hex_number;
                    } else {
                        while((TOKEN_IS_NUMBER(tokenizer->at[0])) || (tokenizer->at[0] == '.')) {
                            internal_tokenizer_advance(tokenizer, 1);
                        }

                        if(tokenizer->at[0] == 'f' || tokenizer->at[0] == 'F') {
                            internal_tokenizer_advance(tokenizer, 1);
                        }

                        // TODO: Handle f being on the end
                        res.type = Token_Type_number;
                    }

                    res.len = (tokenizer->at - res.e);
                } else {
                    res.type = Token_Type_unknown;
                }
            }
        }
    }

    return(res);
}

TOKEN_PUBLIC_DEC char const *token_type_to_string(Token_Type type) {
    char const *res = "";
    switch(type) {
        case Token_Type_unknown: { res = "Token_Type_unknown"; } break;
        case Token_Type_open_paren: { res = "Token_Type_open_paren"; } break;
        case Token_Type_close_paren: { res = "Token_Type_close_paren"; } break;
        case Token_Type_colon: { res = "Token_Type_colon"; } break;
        case Token_Type_double_colon: { res = "Token_Type_double_colon"; } break;
        case Token_Type_semi_colon: { res = "Token_Type_semi_colon"; } break;
        case Token_Type_asterisk: { res = "Token_Type_asterisk"; } break;
        case Token_Type_open_bracket: { res = "Token_Type_open_bracket"; } break;
        case Token_Type_close_bracket: { res = "Token_Type_close_bracket"; } break;
        case Token_Type_open_brace: { res = "Token_Type_open_brace"; } break;
        case Token_Type_close_brace: { res = "Token_Type_close_brace"; } break;
        case Token_Type_open_angle_bracket: { res = "Token_Type_open_angle_bracket"; } break;
        case Token_Type_close_angle_bracket: { res = "Token_Type_close_angle_bracket"; } break;
        case Token_Type_hash: { res = "Token_Type_hash"; } break;
        case Token_Type_double_hash: { res = "Token_Type_double_hash"; } break;
        case Token_Type_assign: { res = "Token_Type_assign"; } break;
        case Token_Type_dereference: { res = "Token_Type_dereference"; } break;
        case Token_Type_comma: { res = "Token_Type_comma"; } break;
        case Token_Type_tilde: { res = "Token_Type_tilde"; } break;
        case Token_Type_period: { res = "Token_Type_period"; } break;
        case Token_Type_ampersand: { res = "Token_Type_ampersand"; } break;
        case Token_Type_inclusive_or: { res = "Token_Type_inclusive_or"; } break;
        case Token_Type_exclusive_or: { res = "Token_Type_exclusive_or"; } break;
        case Token_Type_not: { res = "Token_Type_not"; } break;
        case Token_Type_plus: { res = "Token_Type_plus"; } break;
        case Token_Type_minus: { res = "Token_Type_minus"; } break;
        case Token_Type_divide: { res = "Token_Type_divide"; } break;
        case Token_Type_modulo: { res = "Token_Type_modulo"; } break;
        case Token_Type_increment: { res = "Token_Type_increment"; } break;
        case Token_Type_decrement: { res = "Token_Type_decrement"; } break;
        case Token_Type_plus_equal: { res = "Token_Type_plus_equal"; } break;
        case Token_Type_minus_equal: { res = "Token_Type_minus_equal"; } break;
        case Token_Type_times_equal: { res = "Token_Type_times_equal"; } break;
        case Token_Type_divide_equal: { res = "Token_Type_divide_equal"; } break;
        case Token_Type_equal: { res = "Token_Type_equal"; } break;
        case Token_Type_not_equal: { res = "Token_Type_not_equal"; } break;
        case Token_Type_greater_than_or_equal: { res = "Token_Type_greater_than_or_equal"; } break;
        case Token_Type_less_than_or_equal: { res = "Token_Type_less_than_or_equal"; } break;
        case Token_Type_number: { res = "Token_Type_number"; } break;
        case Token_Type_hex_number: { res = "Token_Type_hex_number"; } break;
        case Token_Type_identifier: { res = "Token_Type_identifier"; } break;
        case Token_Type_string: { res = "Token_Type_string"; } break;
        case Token_Type_char: { res = "Token_Type_char"; } break;
        case Token_Type_var_args: { res = "Token_Type_var_args"; } break;
        case Token_Type_left_shift: { res = "Token_Type_left_shift"; } break;
        case Token_Type_right_shift: { res = "Token_Type_right_shift"; } break;
        case Token_Type_and: { res = "Token_Type_and"; } break;
        case Token_Type_or: { res = "Token_Type_or"; } break;
        case Token_ternary: { res = "Token_ternary"; } break;
        case Token_Type_backslash: { res = "Token_Type_backslash"; } break;
        case Token_Type_error: { res = "Token_Type_error"; } break;
        case Token_Type_end_of_stream: { res = "Token_Type_end_of_stream"; } break;
        case Token_Type_count: { res = "Token_Type_count"; } break;

        default: { TOKEN_ASSERT(0); } break;
    }

    return(res);
}

#endif // defined(TOKEN_IMPLEMENTATION)

} // namespace Tokenizer_Cpp

#endif // !defined(_TOKEN_H)

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
