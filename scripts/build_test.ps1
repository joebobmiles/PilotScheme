# This is the Windows test script

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