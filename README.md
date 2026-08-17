# Simple Shell

## About the Project
This shell is built from scratch in **C** to better understand Linux process management.  
It supports basic command execution, background processes, signal handling, and more — without relying on the `system()` call.

## Features
- **Run Linux commands** in the foreground (e.g., `ls`, `cat`, `echo`, `sleep`, etc.)
- **`cd` command** to change directories within the shell
- **Background execution** with `&`
- **Exit codes** display for failed commands
- **`exit` command** to quit the shell and clean up background processes
- **SIGINT (Ctrl+C) handling** — kills only the foreground process, not the shell or background jobs
- **Serial (`&&`) and parallel (`&&&`) execution** of multiple foreground commands

## How to Run
```bash
gcc my_shell.c -o my_shell
./my_shell
# Linux-Shell
