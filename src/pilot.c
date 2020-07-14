/**
 * This is the core of Pilot Scheme. It receives Pilot Scheme code and outputs
 * intermediate source for code generation.
 */

#include "stdlib.h"
#include "stdio.h"

#include "pilot.h"

#include "buffer.c"

static int IsWhitespace(char Character);

static inline int IsListStart(char Character);
static inline int IsListEnd(char Character);

token NextToken(token_stream* Tokenizer)
{
    // NOTE: Tokenizer could carry the Buffer around, releasing us from having
    // to perform redundant memory operations.
    // An alternative would be to make the Buffer static, so that it persists
    // between NextToken() calls without having to be part of Tokenizer. The
    // only problem with doing that would be that there would be no-one to clean
    // up Buffer when tokenization is done.
    char_buffer Buffer = (char_buffer) { 0 };

    token CurrentToken = (token) { 0 };


    char Character = '\0';
    while ((Character = *(Tokenizer->Cursor)) != '\0')
    {
        if (IsWhitespace(Character))
        {
            if (Buffer.Length > 0)
            {
                CurrentToken.Value = CopyCharBuffer(&Buffer);
                CurrentToken.Type = NAME;

                break;
            }

            Tokenizer->Cursor++;
        }
        else if (IsListStart(Character) || IsListEnd(Character))
        {
            if (Buffer.Length > 0)
            {
                CurrentToken.Value = CopyCharBuffer(&Buffer);
                CurrentToken.Type = NAME;

                break;
            }

            AppendChar(&Buffer, Character);
            CurrentToken.Value = CopyCharBuffer(&Buffer);

            if (IsListStart(Character))
                CurrentToken.Type = LIST_START; 
            else
                CurrentToken.Type = LIST_END;

            Tokenizer->Cursor++;
            break;
        }
        else
        {
            AppendChar(&Buffer, Character);
            Tokenizer->Cursor++;
        }
    }

    FreeCharBuffer(&Buffer);

    return CurrentToken;
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

int IsListStart(char Character) { return Character == '('; }
int IsListEnd(char Character) { return Character == ')'; }
