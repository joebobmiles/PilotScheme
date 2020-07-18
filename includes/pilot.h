#ifndef PILOT_SCHEME_H
#define PILOT_SCHEME_H

/**
 * Pilot Scheme Header File.
 *
 * Include this header file into your C or C++ project to use the Pilot Scheme
 * interpreter.
 */

// Define size_t, if the consumer asks us.
#ifdef PILOT_DEFINE_SIZE_T

#if defined(__SIZE_TYPE__)
typedef __SIZE_TYPE__ __pilot_size_t;
#elif defined(_WIN64)
typedef unsigned long long int __pilot_size_t;
#elif defined(_WIN32)
typedef unsigned long int __pilot_size_t;
#else
#error Unable to define size_t!
#endif

#define size_t __pilot_size_t

#endif // PILOT_DEFINE_SIZE_T

typedef struct {
    char* buffer;
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

/// GLOBAL VARS
static void* arena = 0;
static void* arena_cursor = 0;
static size_t arena_length = 0;

/**
 * Pilot Scheme initialization.
 * 
 * This is the first function you have to call in order to start using the
 * compiler. Pilot Scheme needs a memory pool to allocate over so that we don't
 * have to depend on a custom allocator.
 * 
 * @param   provided_arena  Memory allocated for our use by the consumer.
 * @param   max_size        There's no way in hell we're relying on sentinels.
 */
void
plt_init(void* provided_arena, const size_t max_size)
{
    arena = provided_arena;
    arena_cursor = arena;
    arena_length = max_size;
}


/// MEMORY MANAGEMENT

/**
 * Custom allocator.
 * 
 * A linear allocator with basic boundary detection so we don't muddy the
 * consumer's carpet doing our work.
 * 
 * @param   requested_size  How big do you want it?
 * @return  The start address of the freshly allocated memory.
 */
static void*
allocate(const size_t requested_size)
{
    const size_t used_length = (size_t)arena_cursor - (size_t)arena;

    const size_t actual_allocation_size = requested_size + sizeof(size_t);

    if (arena_length - used_length < actual_allocation_size)
        return 0;

    size_t* allocated_pointer = arena_cursor;
    arena_cursor = (void*)((size_t)arena_cursor + actual_allocation_size);

    *allocated_pointer = requested_size;

    return (void*)((size_t)allocated_pointer + sizeof(size_t));
}

/**
 * Budget memcpy().
 * 
 * Copies data from source to destination. Literally memcpy().
 * 
 * @param   source  Pointer to source data.
 * @param   source_size Better safe than sorry.
 * @param   destination Pointer to data destination.
 */
static void
copy(const char* source, const size_t source_size, char* destination)
{
    for (size_t i = 0; i < source_size; i++)
        destination[i] = source[i];
}

static void*
reallocate(const void* pointer, const size_t new_size)
{
    void* new_pointer = allocate(new_size);

    const size_t* old_size = (size_t*)((size_t)pointer - sizeof(size_t));

    if (pointer)
        copy(pointer, *old_size, new_pointer);

    return new_pointer;
}

/// DYNAMIC BUFFERS

/**
 * Appends a value to the end of a stretchy buffer. After appending, tags a null
 * terminator to the end of the stretchy buffer.
 * 
 * NOTE: The null terminator trick may not work for struct types!
 * 
 * @param b Either a NULL pointer or pointer to a stretchy buffer.
 * @param value The value to append to the stretchy buffer.
 * @return  Zero.
 */
#define buffer_append(b, value) \
    (__buffer_maybe_grow(b, 1), \
    (b)[__buffer_used(b)++] = (value), \
    (b)[__buffer_used(b)] = 0)

/**
 * Returns the number of items currently in the stretchy buffer.
 * 
 * @param b Either a NULL pointer or a pointer to a stretchy buffer.
 * @return  The number of items in the stretchy buffer. (Zero for NULL pointer.)
 */
#define buffer_count(b) ((b) ? __buffer_used(b) : 0)

/**
 * Sets the "used" section of the stretchy buffer header to 0, effectively 
 * resetting the buffer.
 * 
 * @param b Either a NULL pointer or a pointer to a stretchy buffer.
 * @return  Zero.
 */
#define buffer_reset(b) ((b) ? __buffer_used(b) = 0 : 0)

/**
 * Retrieves the raw stretchy buffer as a size_t*. This allows direct access to
 * the 'size' and 'used' values, stored at index 0 and index 1, respectively.
 * 
 * @param b A pointer to a stretchy buffer.
 * @return  The pointer to the raw stretchy buffer.
 */
#define __buffer_raw(b) \
    ((size_t*)((size_t)(b) - (sizeof(size_t) * 2)))

/**
 * Retrieves the 'size' value from a stretchy buffer.
 * 
 * @param b A pointer to a stretchy buffer.
 * @return  The amount of space allocated to the stretchy buffer.
 */
#define __buffer_size(b) __buffer_raw(b)[0]

/**
 * Retrieves the 'used' value from a stretchy buffer.
 * 
 * @param b A pointer to a stretchy buffer.
 * @return  The number of items in the stretchy buffer.
 */
#define __buffer_used(b) __buffer_raw(b)[1]

/**
 * Decides if a stretchy buffer needs to grow to accomodate more elements.
 * 
 * @param b Either a NULL pointer or a pointer to a stretchy buffer.
 * @param increment The number of new elements the buffer needs to accomodate.
 * @return  Zero if buffer has space, or one if buffer needs to be reallocated.
 */
#define __buffer_needs_to_grow(b, increment) \
    ((b) == 0 || __buffer_used(b) + (increment) >= __buffer_size(b))

/**
 * Grows the stretch buffer by calling __buffer_growf() and then assigning the
 * passed buffer to the newly allocated pointer.
 * 
 * @param b Either a NULL pointer or a pointer to a stretchy buffer.
 * @param increment The number of new elements the buffer needs to accomodate.
 * @return  A pointer to the reallocated buffer.
 */ 
#define __buffer_grow(b, increment) \
    (*((void**)&(b)) = __buffer_growf((b), (increment), sizeof(*(b))))

/**
 * Optionally grows the stretchy buffer if the buffer needs to grow.
 * 
 * @param b Either a null pointer or a pointer to a stretchy buffer.
 * @param increment The number of new elements the buffer needs to accomodate.
 * @return  Zero or the pointer to the reallocated buffer.
 */
#define __buffer_maybe_grow(b, increment) \
    (__buffer_needs_to_grow(b, (increment))) \
    ? __buffer_grow(b, (increment)) \
    : 0

/**
 * Grows a stretchy buffer.
 * 
 * @param buffer    Either a NULL pointer or a pointer to a stretchy buffer.
 * @param increment The number of new elements the buffer needs to accomodate.
 * @param item_size The number of bytes an item in the stretchy buffer takes up.
 * @return  A pointer to the newly (re)allocated stretchy buffer.
 */
static void*
__buffer_growf(void* buffer, const unsigned int increment, size_t item_size)
{
    size_t double_current_size = buffer ? 2 * (__buffer_size(buffer)) : 0;
    size_t minimum_needed_size = buffer_count(buffer) + increment;

    // We do this because, when the buffer is empty (i.e. NULL pointer),
    // double_current_size is zero and minimum_needed_size is one. This also
    // allows us to implement a stb_sb_add() macro that grows the buffer by an
    // arbitrary increment, instead of an increment of one.
    size_t new_size =
        double_current_size > minimum_needed_size
        ? double_current_size
        : minimum_needed_size;

    size_t* new_buffer = reallocate(
        buffer ? __buffer_raw(buffer) : 0,
        item_size * new_size + sizeof(size_t) * 2);

    if (new_buffer)
    {
        new_buffer[0] = new_size;

        if (!buffer)
            new_buffer[1] = 0;

        return &new_buffer[2];
    }
    else
    {
        // TODO: OUT OF MEMORY!!!
        return 0;
    }
}

/// LEXING

/**
 * Is whitespace?
 * 
 * A predicate that confirms a character is, in fact, whitespace.
 * TODO(jrm): Make this branchless?
 * 
 * @param   character   The character to check.
 * @return  A poor-man's boolean, 1 if whitespace, 0 otherwise.
 */
static int
is_whitespace(const char character)
{
    const char* whitespace_characters = " \t\r\n";

    for (char whitespace_character = *whitespace_characters;
         whitespace_character != '\0';
         whitespace_character = *(++whitespace_characters))
    {
        if (character == whitespace_character) return 1;
    }

    return 0;
}

/**
 * Is start of list?
 * 
 * A predicate that checks if a character is equal to '(', the traditional Lisp
 * "start of list" character.
 * 
 * @param   character   The character to check.
 * @return  A poor-man's boolean, 1 if '(', 0 otherwise.
 */
static inline int
is_list_start(const char character) { return character == '('; }

/**
 * Is end of list?
 * 
 * A predicate that checks if a character is equal to ')', the traditional Lisp
 * "end of list" character.
 * 
 * @param   character   The character to check.
 * @return  A poor-man's boolean, 1 if ')', 0 otherwise.
 */
static inline int
is_list_end(const char character) { return character == ')'; }

/**
 * Retrieves the next token from the source code provided.
 * 
 * Using the given lexer for context, we grab the next token waiting in the
 * source code. If we're at the end of the source, then an invalid token is
 * returned to the caller.
 * 
 * @param   lexer   A pointer to the current lexer.
 * @param   source  A pointer to the source code we are currently lexing.
 * @param   source_length   How long the entire source code string is.
 * @return  The token extracted from the source.
 */
plt_token
plt_next_token(
    plt_lexer* lexer,
    const char* source,
    const int source_length)
{
    plt_token t;
    t.text = 0;
    t.type = 0;

    while (lexer->cursor_offset < source_length)
    {
        const char c = source[lexer->cursor_offset];

        if (is_whitespace(c))
        {
            if (buffer_count(lexer->buffer) > 0)
            {
                t.text = lexer->buffer;
                t.type = NAME;

                lexer->cursor_offset++;
                break;
            }
        }
        else if (is_list_start(c) || is_list_end(c))
        {
            if (buffer_count(lexer->buffer) > 0)
            {
                t.text = lexer->buffer;
                t.type = NAME;

                break;
            }

            buffer_append(lexer->buffer, c);
            t.text = lexer->buffer;

            if (is_list_start(c))
                t.type = LIST_START;
            else
                t.type = LIST_END;
            
            lexer->cursor_offset++;
            break;
        }
        else
        {
            buffer_append(lexer->buffer, c);
            lexer->cursor_offset++;
        }
    }

    buffer_reset(lexer->buffer);

    return t;
}

/// CLEANUP

// Clean up size_t definition so we don't pollute consumer's namespace.
#if defined(PILOT_DEFINE_SIZE_T) && defined(size_t)
#undef size_t
#endif

// Clean up stretchy buffer defines.
#undef buffer_append
#undef buffer_count
#undef buffer_reset
#undef __buffer_raw
#undef __buffer_size
#undef __buffer_used
#undef __buffer_needs_to_grow
#undef __buffer_grow
#undef __buffer_maybe_grow

#endif
