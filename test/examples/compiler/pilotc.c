#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "pilot.h"

#define KiB(n) (1024 * (n))
#define MiB(n) (1024 * KiB(n))

int
main(void)
{
    const size_t memory_pool_size = MiB(1);
    void* memory_pool = malloc(memory_pool_size);
    memset(memory_pool, 0, memory_pool_size);

    plt_init(memory_pool, memory_pool_size);

    plt_lexer lexer = { 0 };
    plt_token token = { 0 };

    const char* source = "(cons 1 2)";
    size_t source_length = strlen(source);

    while ((token = plt_next_token(
        &lexer,
        source,
        source_length)).type != PLT_TOKEN_INVALID)
    {
        printf(
            "[%s]\t%s\n",
            plt_token_type_to_string(token.type),
            token.text);
    }

    
    return 0;
}