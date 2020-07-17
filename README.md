# 🛫Pilot Scheme

Pilot Scheme is a project that seeks to merge the brilliant but cumbersome
[Racket](https://github.com/racket/racket) (formerly PLT Scheme) and the wicked-
fast [femtolisp](https://github.com/JeffBezanson/femtolisp) -- and wrap it all
into a highly portable, single-header C library!

> This project is still _heavily_ under development!

## Goals

- [ ] Quickest Scheme implementation (compiled _and_ interpreted).
    - [ ] Fast execution times.
    - [ ] Fast compile times.
- [ ] Racket-style macros-making-macros.
- [ ] Compiles to:
    - [ ] C-compatible native code.
    - [ ] Web Assembly.
    - [ ] SPIR-V.
- [ ] C/C++ interop via Foreign Function Interfaces.
    - [ ] 🦄 Julia-like direct calling into C, C++, and Fortran code.
- [ ] Fully embeddable compiler and interpreter.
- [ ] Build automation and package management are handled through Pilot Scheme.
- [ ] Compile-time code execution. (Think Jai's `#run` directive.)
- [ ] Compile-time memory management. (No garbage collector!)
- [ ] Self-hosting.
- [ ] Hot module reloading.

## Non-goals

- [ ] Simple implementation.
- [ ] Become the best-est programming language in all the land.
- [ ] Solve all of software development's big problems.

## Getting Started

### Embedding 🛫Pilot Scheme

Simply include the header `pilot.h` into your project, where ever you like to
keep your headers. Here's an example of how to use Pilot Scheme:

> Presently only the streaming lexer has been implemented, this example
> illustrates its use.

```c
// Allocate a memory pool of 1024B.
size_t memory_pool_size = 1024;
char*  memory_pool = malloc(memory_pool_size);

// Initialize Pilot Scheme with the allocated memory pool.
plt_init(memory_pool, memory_pool_size);

// Zero initialize lexer.
plt_lexer lexer = { 0 };

plt_token t;
do {
    // Grab the next token from the source code.
    t = plt_next_token(
        &lexer,
        source, // Some previously defined const char* to Pilot Scheme source.
        strlen(source)
    )

    // Prints out the text value of the identified token.
    printf("%s\n", t.text);
} while(t.type != INVALID);
```

### Testing 🛫Pilot Scheme

To test Pilot Scheme, run the appropriate `test.*` script for your platform in
the `./scripts/` directory. This will build the tests and produce an executable
in a `./bin/` directory. Running the executable will run all the prepared tests
for Pilot Scheme.

> Only Windows PowerShell is supported at this time. Contributions for build
> scripts on other platforms are welcome!