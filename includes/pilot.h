#ifndef PILOT_SCHEME_H
#define PILOT_SCHEME_H

/**
 * Pilot Scheme Header File.
 *
 * Include this header file into your C or C++ project to use the Pilot Scheme
 * interpreter.
 */

typedef struct {
    char* Value;

    #define TOKEN_TYPES \
        _(NONE) \
        _(LIST_START) \
        _(LIST_END) \
        _(NAME)

    enum {
        #define _(T) T,
        TOKEN_TYPES
        #undef _
    } Type;
} token;

// Pass null-terminated c-string containing Pilot Scheme source code.
// Receive back null-terminated array of tokens.
token* Tokenize(const char*);

#endif
