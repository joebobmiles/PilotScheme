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
        else if (IsDelimiter(Character))
        {
            if (Buffer.Length > 0)
            {
                CurrentToken.Value = CopyCharBuffer(&Buffer);
                CurrentToken.Type = NAME;

                break;
            }

            AppendChar(&Buffer, Character);
            CurrentToken.Value = CopyCharBuffer(&Buffer);

            // TODO: Remove IsDelimiter check?
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

int IsDelimiter(char Character)
{
    return IsListStart(Character) || IsListEnd(Character);
}

int IsListStart(char Character) { return Character == '('; }
int IsListEnd(char Character) { return Character == ')'; }
