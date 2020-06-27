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
    -o .\lib\libpilot.dll `
    .\src\libpilot.c

# Static library build
clang -c `
    -I includes `
    -o .\bin\libpilot.o `
    .\src\libpilot.c

llvm-ar -rc .\lib\libpilot.lib .\bin\libpilot.o
