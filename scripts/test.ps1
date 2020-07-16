# This is the Windows test script

if (Test-Path .\bin)

Push-Location .\bin

clang-cl /Zi `
    /std:c99 `
    /I ..\includes `
    /o .\test.exe `
    ..\test\test.c

Pop-Location