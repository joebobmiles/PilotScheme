/**
 * This is the core of Pilot Scheme. It receives Pilot Scheme code and outputs
 * intermediate source for code generation.
 */

#include "stdlib.h"
#include "stdio.h"

#include "pilot.h"

// #include "buffer.c"


/// GLOBAL VARS
static char* arena = 0;
static char* arena_cursor = 0;
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
plt_init(char* provided_arena, size_t max_size)
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
static char*
allocate(size_t requested_size)
{
    const size_t used_length = arena_cursor - arena;

    if (arena_length - used_length < requested_size)
        return 0;

    void* address = arena;
    arena += requested_size;

    return address;
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
copy(char* source, size_t source_size, char* destination)
{
    for (size_t i = 0; i < source_size; i++)
        destination[i] = source[i];
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
buffer_push(plt_lexer* lexer, char c)
{
    if (lexer->buffer_size == 0)
    {
        lexer->buffer_size = 2;
        lexer->buffer = allocate(sizeof(char) * lexer->buffer_size);
    }
    else if (lexer->buffer_length == lexer->buffer_size)
    {
        char* old_buffer = lexer->buffer;

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
is_whitespace(char character)
{
    char* whitespace_characters = " \t\r\n";

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
is_list_start(char character) { return character == '('; }

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
is_list_end(char character) { return character == ')'; }

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
        char c = source[lexer->cursor_offset];

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
