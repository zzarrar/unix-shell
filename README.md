# Unix Shell

A Unix shell implementation in C++ with process management.

## Features
- Built-in commands (echo, exit, type)
- External command execution with arguments
- PATH search for executables
- Process management using fork/exec/wait

## Build & Run
```bash
g++ main.cpp -o shell
./shell
```

## Implementation
Uses Unix system calls:
- `fork()` - Process creation
- `execve()` - Program execution  
- `waitpid()` - Process synchronization
- `getenv()` - Environment variables

## Tech Stack
- C++
- Unix System Calls
