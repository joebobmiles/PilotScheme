/**
 * This is the core of Pilot Scheme. It receives Pilot Scheme code and outputs
 * intermediate source for code generation.
 */

#include "stdio.h"

#include "pilot.h"

int IsDelimiter(char Character);
int IsWhitespace(char Character);

/**
 * @param   Source  A null-terminated c-string of Pilot Scheme source code.
 * @return  A null-terminated c-string of intermediate source.
 */
char* Compile(char* Source)
{

    for (char Character = *Source;
         Character != '\0';
         Character = *(++Source))
    {
        if (IsWhitespace(Character))
        {
            continue;
        }
        else if (IsDelimiter(Character))
        {
            printf("DELIM\t\"%c\"\n", Character);
        }
        else
        {
            // TODO
        }
    }

    return 0;
}


int IsWhitespace(char Character)
{
    char* WhitespaceCharacters = " \t\r\n\0";

    for (char WhitespaceCharacter = *WhitespaceCharacters;
         WhitespaceCharacter != '\0';
         WhitespaceCharacter = *(++WhitespaceCharacters))
    {
        if (Character == WhitespaceCharacter)
        {
            return 1;
        }
    }

    return 0;
}

int IsDelimiter(char Character)
{
    return Character == '(' || Character == ')';
}
