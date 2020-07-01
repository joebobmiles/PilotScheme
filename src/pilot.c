/**
 * This is the core of Pilot Scheme. It receives Pilot Scheme code and outputs
 * intermediate source for code generation.
 */

#include "stdlib.h"
#include "stdio.h"

#include "pilot.h"

#include "buffer.c"

int IsWhitespace(char Character);

int IsListStart(char Character);
int IsListEnd(char Character);
int IsDelimiter(char Character);

/**
 * @param   Source  A null-terminated c-string of Pilot Scheme source code.
 * @return  A null-terminated c-string of intermediate source.
 */
token* Tokenize(const char* Source)
{
    char_buffer Buffer = (char_buffer)
    {
        .Data = NULL,
        .Length = 0,
        .Size = 0,
    };

    token_buffer Tokens = (token_buffer)
    {
        .Data = NULL,
        .Length = 0,
        .Size = 0,
    };

    token CurrentToken = (token)
    {
        .Value = NULL,
        .Type = NONE,
    };

    for (char Character = *Source;
         Character != '\0';
         Character = *(++Source))
    {
        if (IsWhitespace(Character))
        {
            if (Buffer.Length > 0)
            {
                CurrentToken.Value = CopyCharBuffer(&Buffer);
                CurrentToken.Type = NAME;

                AppendToken(&Tokens, CurrentToken);

                ResetCharBuffer(&Buffer);
            }
        }
        else if (IsDelimiter(Character))
        {
            if (Buffer.Length > 0)
            {
                CurrentToken.Value = CopyCharBuffer(&Buffer);
                CurrentToken.Type = NAME;

                AppendToken(&Tokens, CurrentToken);

                ResetCharBuffer(&Buffer);
            }

            AppendChar(&Buffer, Character);
            CurrentToken.Value = Buffer.Data;

            // TODO: Remove IsDelimiter check?
            if (IsListStart(Character))
                CurrentToken.Type = LIST_START; 
            else
                CurrentToken.Type = LIST_END;

            AppendToken(&Tokens, CurrentToken);

            ResetCharBuffer(&Buffer);
        }
        else
        {
            AppendChar(&Buffer, Character);
        }
    }

    FreeCharBuffer(&Buffer);

    return CopyTokenBuffer(&Tokens);
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
    return IsListStart(Character) || IsListEnd(Character);
}

int IsListStart(char Character) { return Character == '('; }
int IsListEnd(char Character) { return Character == ')'; }
