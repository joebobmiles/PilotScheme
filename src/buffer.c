#ifndef BUFFER_H
#define BUFFER_H

#include "stdlib.h"

#include "pilot.h"


#define BUFFER(T) \
    typedef struct { \
        T* Data; \
        int Length; \
        int Size; \
    } T ## _buffer;

#define APPEND(N, T) \
    int Append ## N(T ## _buffer* Buffer, T Value) { \
        if (Buffer->Size == 0) { \
            Buffer->Size = 2; \
            Buffer->Data = (T*)malloc(sizeof(T) * Buffer->Size); \
        } \
        else if (Buffer->Length >= Buffer->Size) { \
            Buffer->Size *= 2; \
            Buffer->Data = (T*)realloc( \
                Buffer->Data, \
                sizeof(T) * Buffer->Size); \
        } \
        Buffer->Data[Buffer->Length] = Value;\
        return ++Buffer->Length; \
    }

#define FREE(N, T) \
    void Free ## N ## Buffer(T ## _buffer* Buffer) { \
        free(Buffer->Data); \
        Buffer->Size = 0; \
        Buffer->Length = 0; \
    }

#define MAKE_BUFFER(N, T) \
    BUFFER(T) \
    APPEND(N, T) \
    FREE(N, T)


#define BUFFERS \
    _(Char, char) \
    _(Token, token) \


#define _(N, T) MAKE_BUFFER(N, T)
BUFFERS
#undef _


#undef BUFFER
#undef APPEND
#undef MAKE_BUFFER
#undef BUFFERS

#endif
