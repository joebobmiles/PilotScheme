
#include "stdio.h"

#include "pilot.h"

int main(void)
{
    char* String = "()";

    token* Tokens = Tokenize(String);

    for (token Token = *Tokens;
         Token.Value != 0;
         Token = *(++Tokens))
    {
        printf("%s\n", Token.Value);
    }

    return 0;
}
