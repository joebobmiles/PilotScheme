#ifndef BUFFER_H
#define BUFFER_H

#include "stdlib.h"
#include "string.h"

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

#define COPY(N, T) \
    T* Copy ## N ## Buffer(T ## _buffer* Buffer) { \
        T* Copy = (T*)malloc(sizeof(T) * Buffer->Length); \
        memcpy(Copy, Buffer->Data, sizeof(T) * Buffer->Length); \
        Copy[Buffer->Length] = (T) { 0 }; \
        return Copy; \
    }

/*
TODO: We can save on allocations by only resetting length and providing a
Copy*Buffer() function that simply returns a freshly 'alloc'd pointer of the
Buffer type. This would allow us to overwrite the existing buffer without having
to allocate memory for it on the first Append*() operation.
 */
#define RESET(N, T) \
    void Reset ## N ## Buffer(T ## _buffer* Buffer) { \
        Buffer->Data = NULL; \
        Buffer->Length = 0; \
        Buffer->Size = 0; \
    }

#define FREE(N, T) \
    void Free ## N ## Buffer(T ## _buffer* Buffer) { \
        free(Buffer->Data); \
        Reset ## N ## Buffer(Buffer); \
    }

#define MAKE_BUFFER(N, T) \
    BUFFER(T) \
    APPEND(N, T) \
    COPY(N, T) \
    RESET(N, T) \
    FREE(N, T)


#define BUFFERS \
    _(Char, char) \
    _(Token, token) \


#define _(N, T) MAKE_BUFFER(N, T)
BUFFERS
#undef _


#undef BUFFER
#undef APPEND
#undef RESET
#undef FREE
#undef MAKE_BUFFER
#undef BUFFERS

#endif
