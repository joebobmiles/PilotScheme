
#include "stdio.h"

#include "libpilot.h"

int main(void)
{
    char* string = "()";

    if ((int)Compile(string))
        printf("Compiled source code.\n");

    else
        printf("Did not compile source code.\n");

    return (int)Compile(string);
}
