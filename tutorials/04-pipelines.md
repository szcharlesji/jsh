# Chapter 4: Pipelines

Pipelines are one of the most powerful features of Unix shells, allowing you to connect commands together so that the output of one command becomes the input of the next. This enables complex data processing by combining simple commands.

## Understanding Pipelines

A pipeline connects two or more commands using the pipe operator (`|`):

```
command1 | command2 | command3
```

The flow works like this:

1. `command1` produces output
2. That output becomes input for `command2`
3. `command2` processes it and produces new output
4. That output becomes input for `command3`
5. `command3` produces the final result

## Basic Pipeline Syntax

### Two-Command Pipelines

The simplest pipeline connects two commands:

```bash
# Count files in current directory
myproject $ ls | wc -l
8

# Find text files and count them
myproject $ ls | grep "\.txt" | wc -l
3

# Show first few lines of file listing
myproject $ ls -la | head -5
```

### Multiple-Command Pipelines

You can chain multiple commands together:

```bash
# Complex text processing
myproject $ ls -la | grep "\.c$" | wc -l

# Process and format data
myproject $ cat data.txt | sort | uniq | head -10
```

## Common Pipeline Patterns

### Filtering with `grep`

Use `grep` to filter lines containing specific patterns:

```bash
# Find .c files
myproject $ ls | grep "\.c$"
executor.c
jobs.c
main.c
parser.c

# Find files containing "main"
myproject $ ls | grep "main"
main.c
main.o

# Case-insensitive search
myproject $ ls | grep -i "MAKE"
Makefile
```

### Counting with `wc`

Count lines, words, or characters in pipeline output:

```bash
# Count source files
myproject $ ls | grep "\.c$" | wc -l
4

# Count lines in all .h files combined
myproject $ cat *.h | wc -l
47

# Count total characters in file names
myproject $ ls | wc -c
```

### Sorting with `sort`

Sort pipeline output alphabetically or numerically:

```bash
# Sort files by name (default)
myproject $ ls | sort

# Sort files and remove duplicates
myproject $ ls | sort | uniq

# Reverse sort
myproject $ ls | sort -r
```

### Limiting Output with `head` and `tail`

Show first or last few lines of output:

```bash
# Show first 3 files
myproject $ ls | head -3
executor.c
executor.h
executor.o

# Show last 5 files
myproject $ ls | tail -5

# Show first 10 lines of combined headers
myproject $ cat *.h | head -10
```

## Practical Pipeline Examples

### File Analysis

```bash
# Find the largest files (if ls -l works)
myproject $ ls -la | grep "^-" | sort -k5 -n | tail -5

# Count different file types
myproject $ ls | grep -o "\.[^.]*$" | sort | uniq -c

# Find files modified recently
myproject $ ls -la | grep "$(date +%b)"
```

### Text Processing

```bash
# Create sample text file for testing
myproject $ echo "apple" > fruits.txt
myproject $ echo "banana" >> fruits.txt
myproject $ echo "apple" >> fruits.txt
myproject $ echo "cherry" >> fruits.txt

# Sort and remove duplicates
myproject $ cat fruits.txt | sort | uniq
apple
banana
cherry

# Count occurrences of each item
myproject $ cat fruits.txt | sort | uniq -c
   2 apple
   1 banana
   1 cherry
```

### System Information

```bash
# Process information (if ps works)
myproject $ ps aux | grep "jsh" | wc -l

# Find running processes
myproject $ ps aux | head -1 && ps aux | grep -v "grep" | grep "bash"
```

### Log File Analysis

```bash
# Create sample log for testing
myproject $ echo "[INFO] System started" > app.log
myproject $ echo "[ERROR] Connection failed" >> app.log
myproject $ echo "[INFO] User login" >> app.log
myproject $ echo "[ERROR] Database error" >> app.log

# Count error messages
myproject $ cat app.log | grep "ERROR" | wc -l
2

# Show only error messages
myproject $ cat app.log | grep "ERROR"
[ERROR] Connection failed
[ERROR] Database error

# Get unique log levels
myproject $ cat app.log | grep -o "\[.*\]" | sort | uniq
[ERROR]
[INFO]
```

## Pipelines with Redirection

You can combine pipelines with I/O redirection:

### Pipeline Output to File

```bash
# Save filtered results to file
myproject $ ls | grep "\.c$" | sort > source_files.txt

# Append pipeline results
myproject $ ls | grep "\.h$" | sort >> source_files.txt
```

### File Input to Pipeline

```bash
# Process file contents through pipeline
myproject $ cat data.txt | sort | uniq > processed.txt

# Complex processing
myproject $ cat fruits.txt | sort | uniq -c | sort -nr > fruit_counts.txt
```

### Complete Workflows

```bash
# Create comprehensive file analysis
myproject $ ls -la | grep "^-" | wc -l > file_stats.txt
myproject $ echo "Total files:" | cat - file_stats.txt > temp.txt
myproject $ mv temp.txt file_stats.txt
```

## Pipeline Behavior in JSH

### Process Creation

JSH creates separate processes for each command in the pipeline:

```bash
# This creates 3 processes
myproject $ ls | grep "\.c$" | wc -l
```

Each process runs concurrently, connected by pipes.

### Exit Status

The pipeline's exit status is the exit status of the last command:

```bash
# If wc succeeds, pipeline succeeds (even if grep finds nothing)
myproject $ ls | grep "nonexistent" | wc -l
0
# Pipeline succeeds because wc succeeded
```

### Error Handling

If any command in the pipeline fails, JSH reports an error:

```bash
# Invalid command in pipeline
myproject $ ls | invalidcommand | wc -l
Error: invalid command

# Built-in commands cannot be used in pipelines
myproject $ ls | cd /tmp
Error: invalid command
```

## Limitations in JSH

### What Works

- Multiple external commands connected by pipes
- Combining pipelines with I/O redirection
- Any number of commands in a pipeline

### What Doesn't Work

- **Built-in commands in pipelines**: `ls | cd /tmp`
- **Complex operators**: `&&`, `||`, `;`
- **Background pipelines**: `ls | wc -l &`
- **Pipeline branching**: `tee` command functionality

## Advanced Pipeline Techniques

### Data Transformation Chains

```bash
# Multi-step data processing
myproject $ cat data.csv | grep -v "^#" | sort -t',' -k2 | head -10

# Text analysis pipeline
myproject $ cat document.txt | tr ' ' '\n' | grep -v "^$" | sort | uniq -c | sort -nr
```

### Filtering and Selection

```bash
# Complex filtering
myproject $ ls -la | grep "^-" | grep "\.txt$" | sort -k5 -n

# Pattern extraction
myproject $ cat config.txt | grep "=" | grep -v "^#" | sort
```

### Statistical Analysis

```bash
# Count unique values
myproject $ cat data.txt | cut -d',' -f3 | sort | uniq -c | sort -nr

# Basic statistics (if available commands work)
myproject $ cat numbers.txt | sort -n | head -1  # minimum
myproject $ cat numbers.txt | sort -n | tail -1  # maximum
```

## Practice Exercises

### Exercise 1: Basic Pipelines

1. Create a list of files: `ls | head -5`
2. Count source files: `ls | grep "\.c$" | wc -l`
3. Sort files by name: `ls | sort`

### Exercise 2: Text Processing

1. Create a text file with duplicate lines
2. Use pipeline to sort and remove duplicates: `cat file.txt | sort | uniq`
3. Count occurrences: `cat file.txt | sort | uniq -c`

### Exercise 3: Complex Pipelines

1. Create a data file with multiple columns
2. Extract specific columns and process them
3. Save results to a new file using redirection

### Exercise 4: Real-world Scenario

1. Analyze the JSH source code:

   ```bash
   myproject $ ls *.c | wc -l > analysis.txt
   myproject $ ls *.h | wc -l >> analysis.txt
   myproject $ echo "Source analysis complete" >> analysis.txt
   ```

## Troubleshooting

### Common Issues

**"Error: invalid command" in pipelines**

- One of the commands doesn't exist
- Trying to use built-in commands in pipelines
- Check each command individually first

**Empty output from pipeline**

- First command produces no output
- Filter command removes all lines
- Test each stage of the pipeline separately

**Broken pipes**

- Early command exits before later commands finish reading
- Usually not visible in JSH, but may affect complex pipelines

**Memory usage**

- Long pipelines can use significant memory
- Large amounts of data flowing between processes

### Debugging Pipelines

Test each part of the pipeline separately:

```bash
# Test step by step
myproject $ ls
myproject $ ls | grep "\.c$"
myproject $ ls | grep "\.c$" | wc -l
```

## Performance Considerations

### Pipeline Efficiency

- Pipelines run concurrently, improving performance
- Avoid unnecessary intermediate files
- Use appropriate buffer sizes for large data

### Memory Usage

```bash
# Efficient - streaming processing
myproject $ cat largefile.txt | grep "pattern" | head -10

# Less efficient - creates intermediate file
myproject $ cat largefile.txt > temp.txt
myproject $ grep "pattern" temp.txt > temp2.txt
myproject $ head -10 temp2.txt
```

## Summary

In this chapter, you learned:

- ✅ How to connect commands with the pipe operator (`|`)
- ✅ Common pipeline patterns: filtering, counting, sorting
- ✅ Combining pipelines with I/O redirection
- ✅ Practical examples for text processing and analysis
- ✅ JSH's pipeline limitations and behavior
- ✅ Troubleshooting and debugging techniques

Pipelines are essential for Unix command-line productivity, enabling complex data processing by combining simple, focused commands.

## Next Steps

Now that you understand pipelines, you're ready for Chapter 5, where you'll learn about job control - managing background processes and suspended commands.

---

**Previous Chapter**: [← I/O Redirection](03-io-redirection.md)  
**Next Chapter**: [Job Control →](05-job-control.md)

