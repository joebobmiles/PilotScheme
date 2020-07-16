#ifndef PILOT_SCHEME_H
#define PILOT_SCHEME_H

/**
 * Pilot Scheme Header File.
 *
 * Include this header file into your C or C++ project to use the Pilot Scheme
 * interpreter.
 */

/// Define size_t, unless we're told not to.
#ifdef PILOT_INFER_SIZE_T

#if defined(__SIZE_TYPE__)
typedef __SIZE_TYPE__ size_t;
#elif defined(_WIN64)
typedef unsigned long long int size_t;
#elif defined(_WIN32)
typedef unsigned long int size_t;
#else
#error Can't infer size_t! Please define size_t!
#endif

#endif // PILOT_NO_SIZE_T


void
plt_init(char*, size_t);


typedef struct {
     char* buffer;
     unsigned int buffer_length;
     unsigned int buffer_size;
     unsigned int cursor_offset; // Offset into buffer
} plt_lexer;

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
plt_next_token(plt_lexer*, const char*, const int);

#endif
