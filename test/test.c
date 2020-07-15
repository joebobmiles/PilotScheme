
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "pilot.h"

int
main(void)
{
    const char* source = "(cons 1 2)\0";

    /// BEGIN compiler setup
    const int max_arena_size = 1024 * 1024;

    plt_compiler compiler;
    compiler.arena = malloc(max_arena_size);
    compiler.arena_cursor = compiler.arena;
    compiler.arena_length = max_arena_size;

    plt_lexer lexer;
    lexer.buffer = 0;
    lexer.buffer_length = 0;
    lexer.buffer_size = 0;
    lexer.cursor_offset = 0;
    /// END compiler setup
    
    plt_token t;
    do {
        t = plt_next_token(
            &compiler,
            &lexer,
            source,
            strlen(source));

        // TODO(jrm)
        printf("%#010x\t%s\n",
            (unsigned int) t.text,
            t.text);
    } while(t.type != INVALID);

    return 0;
}
