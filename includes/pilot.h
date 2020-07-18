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
plt_init(const void* provided_arena, const size_t max_size)
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

    copy(pointer, *old_size, new_pointer);

    return new_pointer;
}

/// DYNAMIC BUFFERS

/**
 * Push new character onto lexer->buffer.
 * 
 * Pushes a new character onto the end of lexer->buffer. Validates that there's
 * space, and if there is none, either allocates buffer (assuming it's empty),
 * or re-allocates the buffer with a bigger size to accomodate the new character.
 * The buffer is null-terminated for extra safety.
 * 
 * TODO(jrm): Generalize this?
 * 
 * @param   lexer   The lexer being used to perform lexing.
 * @param   c   The character being pushed onto the buffer.
 * @return  The new character count of the buffer.
 */
static int
buffer_push(plt_lexer* lexer, const char c)
{
    if (lexer->buffer_size == 0)
    {
        lexer->buffer_size = 2;
        lexer->buffer = allocate(sizeof(char) * lexer->buffer_size);
    }
    else if (lexer->buffer_length == lexer->buffer_size)
    {
        const char* old_buffer = lexer->buffer;

        lexer->buffer_size *= 2;
        lexer->buffer = allocate(sizeof(char) * lexer->buffer_size);

        copy(old_buffer, lexer->buffer_length, lexer->buffer);
    }

    lexer->buffer[lexer->buffer_length++] = c;
    lexer->buffer[lexer->buffer_length] = '\0';

    return lexer->buffer_length;
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
            if (lexer->buffer_length > 0)
            {
                t.text = lexer->buffer;
                t.type = NAME;

                lexer->cursor_offset++;
                break;
            }
        }
        else if (is_list_start(c) || is_list_end(c))
        {
            if (lexer->buffer_length > 0)
            {
                t.text = lexer->buffer;
                t.type = NAME;

                break;
            }

            buffer_push(lexer, c);
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
            buffer_push(lexer, c);
            lexer->cursor_offset++;
        }
    }

    lexer->buffer_length = 0;

    return t;
}

// Clean up size_t definition so we don't polute consumer's namespace.
#if defined(PILOT_DEFINE_SIZE_T) && defined(size_t)
#undef size_t
#endif

#endif
