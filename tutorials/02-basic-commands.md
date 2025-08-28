# Chapter 2: Basic Commands

JSH includes several built-in commands that are essential for shell operation. Unlike external programs, built-in commands are handled directly by the shell and don't require forking new processes.

## Built-in Commands Overview

JSH provides four built-in commands:

| Command | Purpose | Syntax |
|---------|---------|--------|
| `cd` | Change directory | `cd <directory>` |
| `exit` | Exit the shell | `exit` |
| `jobs` | List active jobs | `jobs` |
| `fg` | Bring job to foreground | `fg <job_number>` |

## The `cd` Command

The `cd` (change directory) command allows you to navigate the filesystem.

### Basic Usage

```bash
# Change to home directory
myproject $ cd /home/username

# Change to a subdirectory
username $ cd Documents

# Change to parent directory
Documents $ cd ..

# Change to root directory
username $ cd /
```

### How the Prompt Changes

Notice how the prompt shows the basename of your current directory:

```bash
myproject $ cd /home/username
username $ cd Documents
Documents $ cd /tmp
tmp $ cd /usr/local/bin
bin $
```

### Error Handling

JSH provides clear error messages for invalid directories:

```bash
# Trying to access non-existent directory
myproject $ cd nonexistent
Error: invalid directory

# Trying to cd without arguments
myproject $ cd
Error: invalid command
```

### Practical Examples

```bash
# Navigate to system directories
myproject $ cd /var/log
log $ cd /etc
etc $ 

# Navigate using absolute paths
etc $ cd /home/user/projects/jsh
jsh $ 

# Go back to previous location (manually)
jsh $ cd /etc
etc $ cd /home/user/projects/jsh
jsh $
```

## The `exit` Command

The `exit` command cleanly terminates the JSH session.

### Basic Usage

```bash
myproject $ exit
```

This returns you to the parent shell (usually bash or zsh).

### Job Checking

JSH prevents you from exiting if there are suspended jobs:

```bash
# If you have suspended jobs:
myproject $ exit
Error: there are suspended jobs

# List jobs to see what's suspended
myproject $ jobs
[1] sleep 100

# Resume or terminate jobs, then exit
myproject $ fg 1
# ... handle the job ...
myproject $ exit
```

### Error Handling

```bash
# exit doesn't accept arguments
myproject $ exit now
Error: invalid command

# Correct usage
myproject $ exit
```

## The `jobs` Command

The `jobs` command lists all currently suspended (stopped) jobs.

### Basic Usage

```bash
# List all jobs
myproject $ jobs

# When no jobs are running
myproject $ jobs
# (no output)
```

### Job Display Format

Jobs are displayed in the format: `[job_number] command`

```bash
myproject $ jobs
[1] sleep 100
[2] cat largefile.txt
[3] find / -name "*.log"
```

### How Jobs Are Created

Jobs are created when you suspend a running command with `Ctrl+Z`:

```bash
# Start a long-running command
myproject $ sleep 100
# Press Ctrl+Z to suspend it
^Z

# Now check jobs
myproject $ jobs
[1] sleep 100
```

### Error Handling

```bash
# jobs doesn't accept arguments
myproject $ jobs -l
Error: invalid command

# Correct usage
myproject $ jobs
```

## The `fg` Command

The `fg` (foreground) command brings a suspended job back to the foreground.

### Basic Usage

```bash
# Bring job number 1 to foreground
myproject $ fg 1
```

### Complete Workflow Example

```bash
# Start a command
myproject $ sleep 60

# Suspend it with Ctrl+Z
^Z

# Check jobs
myproject $ jobs
[1] sleep 60

# Resume the job
myproject $ fg 1
sleep 60
# (continues running until completion)
```

### Error Handling

```bash
# Invalid job number
myproject $ fg 999
Error: invalid job

# Missing job number
myproject $ fg
Error: invalid command

# Too many arguments
myproject $ fg 1 2
Error: invalid command
```

### Job Numbering

Jobs are numbered sequentially starting from 1. When a job completes or is terminated, its number is reused:

```bash
myproject $ jobs
[1] sleep 100
[2] cat file.txt

# Resume job 1
myproject $ fg 1
# Job 1 completes

# Start and suspend another command
myproject $ find / -name "*.txt"
^Z

# Check jobs - new job gets number 1
myproject $ jobs
[1] find / -name "*.txt"  
[2] cat file.txt
```

## External Commands

JSH can also execute external programs found in `/usr/bin/`:

### Common External Commands

```bash
# File operations
myproject $ ls
myproject $ cat filename.txt
myproject $ mkdir newdir
myproject $ touch newfile.txt

# System information
myproject $ ps
myproject $ date
myproject $ whoami
myproject $ pwd

# Text processing
myproject $ grep "pattern" file.txt
myproject $ wc -l file.txt
myproject $ head -5 file.txt
```

### Command Resolution

JSH looks for external commands in these locations:

1. If command contains `/` → Use as absolute/relative path
2. Otherwise → Look in `/usr/bin/`

```bash
# These work (in /usr/bin/)
myproject $ ls
myproject $ cat

# This works (absolute path)
myproject $ /bin/ls

# This might not work (not in /usr/bin/)
myproject $ ./my_program
Error: invalid program
```

## Combining Built-ins with External Commands

You can use built-in and external commands together:

```bash
# Navigate and list
myproject $ cd /tmp
tmp $ ls -la

# Start a job, suspend it, then check
myproject $ grep -r "pattern" /large/directory
^Z
myproject $ jobs
[1] grep -r "pattern" /large/directory
myproject $ cd /home
home $ fg 1
# grep continues from /large/directory
```

## Command Syntax Rules

### General Rules

1. Commands are case-sensitive
2. Arguments are separated by spaces
3. Leading/trailing spaces are ignored
4. Empty commands are ignored

### Built-in Command Restrictions

- Built-in commands cannot be used in pipelines
- They don't support I/O redirection
- They execute immediately in the current shell process

```bash
# This works
myproject $ cd /tmp

# This doesn't work (pipeline with builtin)
myproject $ echo "/tmp" | cd
Error: invalid command

# External commands work in pipelines
myproject $ ls | wc -l
```

## Practice Exercises

### Exercise 1: Basic Navigation

1. Start JSH
2. Use `cd` to navigate to `/tmp`
3. Use `cd` to go to your home directory
4. Use `cd` to go back to the JSH project directory
5. Exit JSH

### Exercise 2: Job Control Practice

1. Start JSH
2. Run `sleep 30`
3. Suspend it with `Ctrl+Z`
4. Use `jobs` to list it
5. Use `fg 1` to resume it
6. Let it complete or interrupt with `Ctrl+C`

### Exercise 3: Command Exploration

1. Try various external commands: `ls`, `pwd`, `date`, `whoami`
2. Attempt invalid commands and observe error messages
3. Try built-in commands with wrong syntax

## Troubleshooting

### Common Issues

**"Error: invalid program"**
- Command not found in `/usr/bin/`
- Try using full path: `/bin/command`

**"Error: invalid command"**
- Wrong syntax for built-in command
- Check command spelling and arguments

**"Error: there are suspended jobs"**
- Can't exit with active jobs
- Use `jobs` to list them, `fg` to resume them

**Commands don't work in pipelines**
- Built-in commands can't be piped
- Use external equivalents when available

## Summary

In this chapter, you learned:

- ✅ How to use `cd` to navigate directories
- ✅ How to safely `exit` JSH
- ✅ How to list suspended jobs with `jobs`
- ✅ How to resume jobs with `fg`
- ✅ The difference between built-in and external commands
- ✅ Common error messages and their solutions

## Next Steps

Now that you understand basic commands, you're ready to learn about I/O redirection in Chapter 3, where you'll discover how to:

- Redirect command output to files
- Read input from files
- Chain commands together

---

**Previous Chapter**: [← Getting Started](01-getting-started.md)  
**Next Chapter**: [I/O Redirection →](03-io-redirection.md)