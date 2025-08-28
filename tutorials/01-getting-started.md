# Chapter 1: Getting Started with JSH

Welcome to JSH, a simple yet functional Unix shell implementation! This tutorial will guide you through installing, building, and running your first commands in JSH.

## What is JSH?

JSH (Just Shell) is a lightweight Unix shell written in C that provides essential shell functionality. It's designed as an educational project to understand how shells work internally, featuring:

- Command execution
- Built-in commands
- I/O redirection
- Pipelines
- Job control

## Prerequisites

Before you begin, ensure you have:

- A Unix-like system (Linux, macOS, WSL)
- GCC compiler installed
- Make utility
- Basic familiarity with the command line

### Checking Prerequisites

```bash
# Check if GCC is installed
gcc --version

# Check if Make is installed
make --version
```

If these commands fail, install the development tools for your system:

**Ubuntu/Debian:**

```bash
sudo apt update
sudo apt install build-essential
```

**macOS:**

```bash
xcode-select --install
```

**CentOS/RHEL:**

```bash
sudo yum groupinstall "Development Tools"
```

## Building JSH

### Step 1: Navigate to the Project Directory

```bash
cd /path/to/jsh
```

### Step 2: Build the Project

```bash
make
```

This will compile all source files and create the `jsh` executable. You should see output similar to:

```
gcc -Wall -Wextra -std=c99 -pedantic -g -c main.c -o main.o
gcc -Wall -Wextra -std=c99 -pedantic -g -c parser.c -o parser.o
gcc -Wall -Wextra -std=c99 -pedantic -g -c executor.c -o executor.o
gcc -Wall -Wextra -std=c99 -pedantic -g -c jobs.c -o jobs.o
gcc -Wall -Wextra -std=c99 -pedantic -g -o jsh main.o parser.o executor.o jobs.o
```

### Step 3: Verify the Build

```bash
ls -la jsh
```

You should see the `jsh` executable with execute permissions.

## Running JSH

### Starting the Shell

```bash
./jsh
```

You should see a prompt showing your current directory name:

```
myproject $
```

The prompt format is: `[directory_basename] $`

### Your First Commands

Let's try some basic commands to ensure JSH is working:

```bash
# Check current directory
myproject $ pwd

# List files in current directory
myproject $ ls

# Try a simple command
myproject $ echo "Hello, JSH!"
```

### Exiting JSH

To exit the shell:

```bash
myproject $ exit
```

This returns you to your original shell.

## Understanding the Build Process

### Source Files

JSH consists of several C source files:

- `main.c` - Main shell loop and user interface
- `parser.c` - Command parsing and built-in commands
- `executor.c` - External command execution
- `jobs.c` - Job control and signal handling

### Header Files

- `jsh.h` - Common definitions and constants
- `parser.h` - Parser function declarations
- `executor.h` - Executor function declarations
- `jobs.h` - Job control function declarations

### Makefile Targets

```bash
# Build everything (default)
make

# Clean build artifacts
make clean

# Rebuild from scratch
make clean && make
```

## Troubleshooting Common Issues

### Build Errors

**Error: `gcc: command not found`**

- Solution: Install GCC compiler (see Prerequisites section)

**Error: `make: command not found`**

- Solution: Install Make utility

**Error: Permission denied when running `./jsh`**

- Solution: Make sure the file is executable: `chmod +x jsh`

### Runtime Issues

**Commands not found**

- JSH looks for external commands in `/usr/bin/`
- Built-in commands (`cd`, `exit`, `jobs`, `fg`) should always work
- Try using full paths: `/bin/ls` instead of `ls`

**Shell doesn't respond to Ctrl+C**

- This is normal behavior - JSH handles signals differently
- Use `exit` command to quit

## Next Steps

Now that you have JSH running, you're ready to explore its features:

1. **Chapter 2**: Learn about built-in commands (`cd`, `exit`, `jobs`, `fg`)
2. **Chapter 3**: Understand I/O redirection (`>`, `>>`, `<`)
3. **Chapter 4**: Master pipelines (`|`)
4. **Chapter 5**: Control jobs and processes

## Practice Exercises

Before moving to the next chapter, try these exercises:

1. **Build and run JSH** following the steps above
2. **Navigate directories** using `cd` (covered in Chapter 2)
3. **Run simple commands** like `ls`, `pwd`, `date`
4. **Exit and restart** JSH several times

## Summary

In this chapter, you learned how to:

- ✅ Check prerequisites for building JSH
- ✅ Build JSH using Make
- ✅ Run JSH and see the prompt
- ✅ Execute basic commands
- ✅ Exit the shell properly
- ✅ Troubleshoot common issues

You're now ready to explore JSH's built-in commands in Chapter 2!

---

**Next Chapter**: [Basic Commands →](02-basic-commands.md)

