#ifndef PILOT_SCHEME_H
#define PILOT_SCHEME_H

/**
 * Pilot Scheme Header File.
 *
 * Include this header file into your C or C++ project to use the Pilot Scheme
 * interpreter.
 */

typedef struct {
     char* buffer;
     unsigned int buffer_length;
     unsigned int buffer_size;
     unsigned int cursor_offset; // Offset into buffer
} plt_lexer;

typedef struct {
    char* arena;
    char* arena_cursor;
    size_t arena_length;
} plt_compiler;

typedef struct {
    const char* text;

    #define TOKEN_TYPES \
        _(INVALID) \
        _(LIST_START) \
        _(LIST_END) \
        _(NAME)

    enum {
        #define _(T) T,
        TOKEN_TYPES
        #undef _
    } type;
} plt_token;

plt_token
plt_next_token(plt_compiler*, plt_lexer*, const char*, const int);

#endif
