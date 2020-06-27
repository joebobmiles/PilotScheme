# This is the Windows test script

.\scripts\lib-build.ps1

clang `
    -std=c99 `
    -I includes `
    -o .\bin\test.exe `
    .\test\test.c .\lib\pilot.lib

.\bin\test.exe
