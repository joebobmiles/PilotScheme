
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "pilot.h"

#include "utest.h"

UTEST(basic, first_token_invalid_for_empty_input)
{
    const size_t memory_pool_size = 1;
    void* memory_pool = malloc(memory_pool_size);
    memset(memory_pool, 0, memory_pool_size);

    plt_init(memory_pool, memory_pool_size);

    plt_lexer lexer = { 0 };
    plt_token token = plt_next_token(
        &lexer,
        "",
        0);

    ASSERT_EQ(token.type, INVALID);

    free(memory_pool);
}

UTEST(lexing, identifies_open_paren)
{
    const size_t memory_pool_size = 1;
    void* memory_pool = malloc(memory_pool_size);
    memset(memory_pool, 0, memory_pool_size);

    plt_init(memory_pool, memory_pool_size);

    plt_lexer lexer = { 0 };
    plt_token token = plt_next_token(
        &lexer,
        "(",
        1);

    ASSERT_EQ(token.type, LIST_START);

    free(memory_pool);
}

UTEST(lexing, identifies_close_paren)
{
    const size_t memory_pool_size = 1;
    void* memory_pool = malloc(memory_pool_size);
    memset(memory_pool, 0, memory_pool_size);

    plt_init(memory_pool, memory_pool_size);

    plt_lexer lexer = { 0 };
    plt_token token = plt_next_token(
        &lexer,
        ")",
        1);

    ASSERT_EQ(token.type, LIST_END);

    free(memory_pool);
}

UTEST(lexing, identifies_name)
{
    const size_t memory_pool_size = 1;
    void* memory_pool = malloc(memory_pool_size);
    memset(memory_pool, 0, memory_pool_size);

    plt_init(memory_pool, memory_pool_size);

    plt_lexer lexer = { 0 };
    plt_token token = plt_next_token(
        &lexer,
        "cons",
        1);

    ASSERT_EQ(token.type, NAME);

    free(memory_pool);
}

UTEST_MAIN()