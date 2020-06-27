# This is the Windows build script for Pilot Scheme 

if (-Not (Test-Path .\bin)) {
    md .\bin | Out-Null
}

if (-Not (Test-Path .\lib)) {
    md .\lib | Out-Null
}

# Shared library build
clang -shared `
    -I includes `
    -o .\lib\pilot.dll `
    .\src\pilot.c

# Static library build
clang -c `
    -I includes `
    -o .\bin\pilot.o `
    .\src\pilot.c

llvm-ar -rc .\lib\pilot.lib .\bin\pilot.o
