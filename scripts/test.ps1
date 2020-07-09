# This is the Windows test script

.\scripts\lib-build.ps1

Push-Location .\bin

clang-cl /Zi `
    /std:c99 `
    /I ..\includes `
    /o .\test.exe `
    ..\test\test.c ..\lib\pilot.lib

# .\test.exe

Pop-Location