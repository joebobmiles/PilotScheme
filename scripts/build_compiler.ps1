# This is the Windows build script for Pilot Scheme example compiler.

if (-not (Test-Path .\bin)) {
    md .\bin | Out-Null
}

Push-Location .\bin

clang-cl /Zi `
    /std:c99 `
    /I ..\includes `
    /o .\pilotc.exe `
    ..\test\examples\compiler\pilotc.c

Pop-Location