
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "pilot.h"

int
main(void)
{
    const char* source = "(cons 1 2)\0";

    /// BEGIN compiler setup
    const size_t max_arena_size = 1024 * 1024;
    char* arena = malloc(max_arena_size);

    plt_init(arena, max_arena_size);

    plt_lexer lexer;
    lexer.buffer = 0;
    lexer.buffer_length = 0;
    lexer.buffer_size = 0;
    lexer.cursor_offset = 0;
    /// END compiler setup
    
    plt_token t;
    do {
        t = plt_next_token(
            &lexer,
            source,
            strlen(source));

        // TODO(jrm)
        printf("%#010x\t%s\n",
            (unsigned int) t.text,
            t.text);
    } while(t.type != INVALID);

    /// Compiler teardown
    free(arena);

    return 0;
}
