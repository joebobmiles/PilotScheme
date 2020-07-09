# This is the Windows build script for Pilot Scheme 

if (-Not (Test-Path .\bin)) {
    md .\bin | Out-Null
}

if (-Not (Test-Path .\lib)) {
    md .\lib | Out-Null
}

Push-Location .\lib

# Shared library build
# Using /LDd (debug DLL) instead of /LD (release DLL)
clang-cl /Zi /LDd `
    /I ..\includes `
    /o .\pilot.dll `
    ..\src\pilot.c

Pop-Location

Push-Location .\bin

# Static library build
clang-cl /Zi /c `
    /I ..\includes `
    /o .\pilot.o `
    ..\src\pilot.c

Pop-Location

Push-Location .\lib

llvm-ar -rc .\pilot.lib ..\bin\pilot.o

Pop-Location