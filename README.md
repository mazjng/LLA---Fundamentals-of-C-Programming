# KV Project

This project is a key value store that defines the following interface

```c
kv_t   *kv_init(size_t capacity);
int     kv_put(kv_t *db, const char *key, const char *value);
char *  kv_get(kv_t *db, const char *key);
int     kv_delete(kv_t, *db, const char *key);
void    kv_free(kv_t *db)
```

# Notes

## Section 2

### Variable Types

| Type | Bits |
---
| (unsigned) char | 8 |
| (unsigned) short | 16 |
| (unsigned) int | 16 or 32 |
| (unsigned) long | 32 |
| (unsigned) long long | 64 |
| float | 32 |
| double | 64 |

For an unsinged type the range is from 0 to 2^(# bits)
For a signed type the range is from -2^(# bits -1) to 2^(# bits) -1, note the
-1 which accomodates 0.

### Type casting

- Signedness Casting

```c
int other_var = -1;
unsigned int x = (unsigned int)other_var;
```

Note that in this example -1 becomes MAX_INT.

- Up Casting

```c
short other_var = -1; // 0xffff or 1111
int x = (int)other_var; // 0xffffffff
```

When you cast up a variable from one size to a larger size, the signedness
comes with it. In this example the value of -1 will be sign extended through
the larger variable.

etc. etc.

## Section 3

### Memory

Virtual vs Physical Memory

- Virtual address: a logical address translated to a physical address at runtime
- Physical address: the address the piece of memory physically in RAM.

Allows the OS to allocate memory to each process.

### 64-bit Linux Process Memory Map

In a 64-bit Linux env, a process memory map typically inclues:

- Text Segment: Contains executable code
- Initialized Data Segment: Stores global and static variables initialized by
the program.
- Uninitialised Data Segment (BSS): Holds uninitialised global and statics
- Heap: Dynamically allocated memory managed by malloc, realloc and free
- Stack: Memory for function call frames, including local variabled and return
addresses.
- Memory-mapped Regions: Used for shared libs and files mapped into memory.

In C,
- Stack Memory: Automatically allocated and deallocated for local variables and
function calls.
- Heap Memory: Dynamically allocated using functions like malloc and free.

### Inspecting memory in Linux

> cat /proc/self/maps

Here proc is the 'proc_fs' process file system, and self refers to the current 
process id. ie, this is opening the map for the current cat process.
Map shows us the memory address range, note that these are page boundaries, in
linux a page is 4096 bytes, the permissions, etc, and the source of the page.

Observe that the sources are the program itself, then heap, the libc, then
somemore things and then the stack at the bottom.

### Strings

Note that in C, strings need "double quotes", 'single quotes' are reserved for 
chars. The null terminator is 0x0.

Two ways to define strings,

```c
char *string = "hello";
```

reserves a ready only, immutable section of data in the .rodata section of the 
ELF. Attempting to mutate string will result in a seg fault.

```c
char string[] = "hello";
```

This allocates memory on the stack, i.e. has local scope and is mutable.

## Section 5

### Union Types

```c
union myunion {
    int i;
    char c;
}
```

In memory, both of them point to the same location. The size of the union is 
determined by the size of the largest element.

Typically, the best practice is to create a structure that wraps the union
to determine what mode the data is being used in.

```c
struct mycontainer {
    mode_t mode;
    union myunion u;
}
```

here mode_t is an enum.

### Static

```c
int static_test() {
    static int = 0;
    i ++;
}
```

A static variable exists in 'static' lifetime, it does not have global scopee.
The variable outlives the lifetime of the function call. The lifetime of the 
variable is the entire execution of the program.

## Section 6

### Valgrind

Compile with debug symbols using the -g flag in gcc

> gcc -0 program code.c -g

To test run

> valgrind --leak-check=full ./program

## Section 7 

### Error handling

glibc has a global variable 

```c
ERRNO
```

that functions write to with the appropriate error codes which can be found on
MAN pages.

```c

int fd = open("./file", 0_RDONLY);
if (fd == -1) {
    perror("open") // prints the current error
    return -1
}

```

### Preprocessor

Touches lines that begin with the # symbol.
Here are some of the most common preprocessor directives

- \#define - Defines a macro

```c
#define PI 3.14
```

- \#undef - Undefines a previously defined macro

```c
#undef PI
```

- \#include - Includes the contents of a file

```c
#include <stdio.h>
#include "myheader.h"
```

- \#if \#elif \#else \#endif - Conditionals
```c
#if DEBUG
printf("debug mode\n)
#endif
```

- \#ifdef, \#ifndef - Checks if a macro is defined or not

```c
#ifdef DEBUG
#define LOG(x) printf(x)
#else
#define LOG(x)
#endif
```

- \#error - Produces a compile-time error

```c
#error "unsupported platform"
```

- \#pragma - Provides compiler-specific instructions

```c
#pragma once
```

- \#line - Changes the compiler's idea of the line number and filename

```c
#line 100 "custom_file.c"
```

- \#warning - Produces a compiler warning (GCC specific)

```c
#warning "This is a warning"
```

## Section 8

### Intro to File Descriptors

In linux, every program has three file descriptions open by default

- 0: stdin - a file that represents the input to your program from the command 
line

- 1: stdout - a file that represents the output of your program to the command
line

- 2: stderr - a file that represents the output of your program, but containing
only errors

### open, read, stat

A stat buffer contains the information / stats of a file.
Note these are POSIX calls and not cross platform.


## Section 10 - Key Value Store

### Hashing Keys

two cases:

- place is free
- collision
- recent deletion (TOMBSTONE)
