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

void
plt_init(char* provided_arena, size_t max_size)
{
    arena = provided_arena;
    arena_cursor = arena;
    arena_length = max_size;
}



// TODO(jrm): convert to branchless?
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

static inline int
is_list_start(char character) { return character == '('; }

static inline int
is_list_end(char character) { return character == ')'; }

//! HACK(jrm): Dumb allocator doing dumb things!
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

static int
copy(char* source, size_t source_size, char* destination)
{
    for (size_t i = 0; i < source_size; i++)
        destination[i] = source[i];

    return 0;
}

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
        size_t old_buffer_size = lexer->buffer_size;
        char* old_buffer = lexer->buffer;

        lexer->buffer_size *= 2;
        lexer->buffer = allocate(sizeof(char) * lexer->buffer_size);

        copy(old_buffer, old_buffer_size, lexer->buffer);
    }

    lexer->buffer[lexer->buffer_length++] = c;
    lexer->buffer[lexer->buffer_length] = '\0';

    return lexer->buffer_length;
}

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
