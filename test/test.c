
#include "stdio.h"

#include "pilot.h"

int main(void)
{
    const char* String = "(cons 1 2)";

    /*
    token* Tokens = Tokenize(String);

    for (token Token = *Tokens;
         Token.Value != 0;
         Token = *(++Tokens))
    {
        printf("%s\n", Token.Value);
    }
    */

    token_stream Tokenizer = (token_stream)
    {
        .Cursor = String,
    };

    token Token;
    while ((Token = NextToken(&Tokenizer)).Value != NULL)
    {
        printf("%#010x\t%s\n",
            (unsigned int)Token.Value,
            Token.Value);

        // printf("%s", Token.Value);
    }

    // printf("\n");

    return 0;
}
