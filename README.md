# JSH - A Simple Unix Shell

JSH is a lightweight Unix shell implementation written in C that provides essential shell functionality including command execution, pipelines, I/O redirection, and job control.

## Features

- **Command Execution**: Execute external programs and built-in commands
- **Built-in Commands**: `cd`, `exit`, `jobs`, `fg`
- **I/O Redirection**: Output redirection (`>`, `>>`), input redirection (`<`)
- **Pipelines**: Chain commands together with pipes (`|`)
- **Job Control**: Suspend jobs with Ctrl+Z and resume with `fg`
- **Signal Handling**: Proper handling of SIGINT, SIGQUIT, and SIGTSTP
- **Memory Management**: Careful memory allocation and cleanup

## Quick Start

### Building JSH

```bash
# Clone or navigate to the project directory
cd jsh

# Build the shell
make

# Run JSH
./jsh
```

### Basic Usage

```bash
# Navigate directories
mydir $ cd /home/user
user $ pwd
/home/user

# List files
user $ ls -la

# Redirect output to file
user $ ls > files.txt

# Use pipes to chain commands
user $ cat files.txt | grep "\.txt" | wc -l

# Run commands in background (Ctrl+Z to suspend)
user $ sleep 100
^Z
[1] sleep 100

# List and resume jobs
user $ jobs
[1] sleep 100
user $ fg 1
```

## Architecture

JSH is built with a modular architecture:

- **main.c**: Main shell loop and user interface
- **parser.c**: Command line parsing and built-in command execution
- **executor.c**: External command execution and pipeline handling
- **jobs.c**: Job control and signal management

## File Structure

```
jsh/
├── main.c          # Main shell loop
├── parser.c        # Command parsing
├── executor.c      # Command execution
├── jobs.c          # Job control
├── jsh.h           # Common definitions
├── parser.h        # Parser interface
├── executor.h      # Executor interface
├── jobs.h          # Job control interface
├── Makefile        # Build configuration
├── README.md       # This file
└── tutorials/      # Detailed tutorials
    ├── 01-getting-started.md
    ├── 02-basic-commands.md
    ├── 03-io-redirection.md
    ├── 04-pipelines.md
    ├── 05-job-control.md
    ├── 06-advanced-usage.md
    └── 07-architecture.md
```

## Compilation Requirements

- GCC compiler
- POSIX-compliant system (Linux, macOS, etc.)
- Standard C library

## Supported Commands

### Built-in Commands

| Command | Description | Usage |
|---------|-------------|--------|
| `cd` | Change directory | `cd <directory>` |
| `exit` | Exit the shell | `exit` |
| `jobs` | List active jobs | `jobs` |
| `fg` | Bring job to foreground | `fg <job_number>` |

### External Commands

JSH can execute any external program available in `/usr/bin/` or specified with an absolute path.

## I/O Redirection

| Operator | Description | Example |
|----------|-------------|---------|
| `>` | Redirect stdout to file (overwrite) | `ls > output.txt` |
| `>>` | Redirect stdout to file (append) | `echo "text" >> file.txt` |
| `<` | Redirect stdin from file | `sort < input.txt` |
| `|` | Pipe output to next command | `ps aux | grep vim` |

## Limitations

- No background job execution with `&`
- No environment variable support
- No command history
- No tab completion
- No wildcard expansion
- Limited to `/usr/bin/` for external commands
- No command substitution or complex expansions

## Error Handling

JSH provides clear error messages for common issues:

- `Error: invalid command` - Command not found or builtin syntax error
- `Error: invalid directory` - Directory doesn't exist for `cd`
- `Error: invalid job` - Job number doesn't exist for `fg`
- `Error: cannot open file` - File I/O redirection failed
- `Error: there are suspended jobs` - Trying to exit with active jobs

## Contributing

This is a simple educational shell implementation. Key areas for improvement:

1. Environment variable support
2. Command history and line editing
3. Background job execution (`&`)
4. Wildcard expansion
5. More built-in commands
6. Better PATH handling

## Tutorials

For detailed learning, see the tutorials directory:

1. **[Getting Started](tutorials/01-getting-started.md)** - Installation and first steps
2. **[Basic Commands](tutorials/02-basic-commands.md)** - Built-in commands
3. **[I/O Redirection](tutorials/03-io-redirection.md)** - File redirection
4. **[Pipelines](tutorials/04-pipelines.md)** - Connecting commands
5. **[Job Control](tutorials/05-job-control.md)** - Managing processes
6. **[Advanced Usage](tutorials/06-advanced-usage.md)** - Complex examples
7. **[Architecture](tutorials/07-architecture.md)** - Code internals

## License

This project is provided as-is for educational purposes.

