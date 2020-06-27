/**
 * This is the core of Pilot Scheme. It receives Pilot Scheme code and outputs
 * intermediate source for code generation.
 */

#include "libpilot.h"

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
            // TODO
        }
        else
        {
            // TODO
        }
    }

    return 0;
}


int IsDelimiter(char Character)
{
    return 0;
}

int IsWhitespace(char Character)
{
    return 0;
}
