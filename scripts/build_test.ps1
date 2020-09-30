# This is the Windows build script for Pilot Scheme unit tests.

if (-not (Test-Path .\bin)) {
    md .\bin | Out-Null
}

Push-Location .\bin

clang-cl /Zi `
    /std:c99 `
    /I ..\includes `
    /o .\test.exe `
    ..\test\test.c

Pop-Location
