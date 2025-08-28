# Chapter 3: I/O Redirection

I/O redirection is one of the most powerful features of Unix shells, allowing you to control where commands read their input from and where they send their output. JSH supports the essential redirection operators for files.

## Understanding Standard Streams

Every command has three standard streams:

- **stdin (0)**: Standard input - where commands read data from
- **stdout (1)**: Standard output - where commands write normal output
- **stderr (2)**: Standard error - where commands write error messages

By default:
- stdin reads from keyboard
- stdout writes to terminal
- stderr writes to terminal

## Output Redirection

### Basic Output Redirection (`>`)

The `>` operator redirects stdout to a file, **overwriting** the file if it exists.

```bash
# Redirect ls output to a file
myproject $ ls > files.txt

# Check the file contents
myproject $ cat files.txt
executor.c
executor.h
executor.o
jsh
jsh.h
jobs.c
jobs.h
jobs.o
main.c
main.o
Makefile
parser.c
parser.h
parser.o
```

### Key Behaviors

1. **File Creation**: Creates the file if it doesn't exist
2. **Overwriting**: Replaces entire file contents
3. **Empty Output**: Creates empty file if command produces no output

```bash
# Create empty file
myproject $ echo "" > empty.txt

# Overwrite existing file
myproject $ date > timestamp.txt
myproject $ echo "New content" > timestamp.txt
myproject $ cat timestamp.txt
New content
```

### Append Output Redirection (`>>`)

The `>>` operator redirects stdout to a file, **appending** to the file if it exists.

```bash
# Create initial file
myproject $ echo "First line" > log.txt

# Append more content
myproject $ echo "Second line" >> log.txt
myproject $ echo "Third line" >> log.txt

# View complete file
myproject $ cat log.txt
First line
Second line
Third line
```

### Practical Examples

```bash
# Save system information
myproject $ date > system_info.txt
myproject $ whoami >> system_info.txt
myproject $ pwd >> system_info.txt

# Build a file list over time
myproject $ ls *.c > source_files.txt
myproject $ ls *.h >> source_files.txt
myproject $ cat source_files.txt
executor.c
jobs.c
main.c
parser.c
executor.h
jobs.h
jsh.h
parser.h
```

## Input Redirection (`<`)

The `<` operator redirects stdin to read from a file instead of the keyboard.

### Basic Usage

```bash
# Create a file with some content
myproject $ echo "hello world" > input.txt
myproject $ echo "this is a test" >> input.txt

# Use the file as input to a command
myproject $ wc -l < input.txt
2

# Sort the contents of a file
myproject $ sort < input.txt
hello world
this is a test
```

### Commands That Read from stdin

Many Unix commands read from stdin by default:

```bash
# wc counts lines, words, characters
myproject $ wc < input.txt
   2    6   27

# grep searches for patterns
myproject $ grep "test" < input.txt
this is a test

# cat displays file contents
myproject $ cat < input.txt
hello world
this is a test
```

### Creating Test Files

```bash
# Create test data for input redirection
myproject $ echo "apple" > fruits.txt
myproject $ echo "banana" >> fruits.txt
myproject $ echo "cherry" >> fruits.txt
myproject $ echo "apple" >> fruits.txt

# Use as input to various commands
myproject $ sort < fruits.txt
apple
apple
banana
cherry

myproject $ uniq < fruits.txt
apple
banana
cherry
apple
```

## Combining Redirections

You can use input and output redirection together:

```bash
# Sort a file and save the result
myproject $ sort < input.txt > sorted.txt

# Process file and append to log
myproject $ wc -l < data.txt >> count_log.txt

# Create sorted unique list
myproject $ sort < fruits.txt > temp.txt
myproject $ uniq < temp.txt > unique_fruits.txt
```

## Redirection with Various Commands

### File Operations

```bash
# Create files with specific content
myproject $ echo "Configuration data" > config.txt
myproject $ date >> config.txt

# Copy file using redirection
myproject $ cat < original.txt > copy.txt
```

### Text Processing

```bash
# Count lines in files
myproject $ wc -l < *.txt > line_counts.txt

# Find patterns and save results
myproject $ grep "error" < logfile.txt > errors.txt

# Process text files
myproject $ head -5 < largefile.txt > first_lines.txt
myproject $ tail -5 < largefile.txt > last_lines.txt
```

### Data Analysis

```bash
# Create sample data
myproject $ echo "100" > numbers.txt
myproject $ echo "200" >> numbers.txt
myproject $ echo "150" >> numbers.txt

# Process the data
myproject $ sort -n < numbers.txt > sorted_numbers.txt
myproject $ cat sorted_numbers.txt
100
150
200
```

## Error Handling

### File Permissions

```bash
# Try to redirect to a directory (fails)
myproject $ echo "test" > /
Error: cannot open file /

# Try to read from non-existent file
myproject $ cat < nonexistent.txt
Error: cannot open file nonexistent.txt
```

### Invalid Syntax

JSH validates redirection syntax:

```bash
# Missing filename
myproject $ ls >
Error: invalid command

# Multiple redirections of same type not supported
myproject $ ls > file1.txt > file2.txt
Error: invalid command
```

## Redirection vs. Pipelines

Redirection works with **files**, while pipelines work with **other commands**:

```bash
# Redirection - output goes to file
myproject $ ls > filelist.txt

# Pipeline - output goes to another command (covered in Chapter 4)
myproject $ ls | wc -l
```

## Practical Workflows

### Log File Management

```bash
# Start a log file
myproject $ date > activity.log
myproject $ echo "Starting work session" >> activity.log

# Add entries throughout your work
myproject $ echo "Compiled project" >> activity.log
myproject $ echo "Fixed bug in parser" >> activity.log

# Review the log
myproject $ cat activity.log
```

### Data Processing Pipeline

```bash
# Create raw data
myproject $ echo "John,25,Engineer" > employees.csv
myproject $ echo "Jane,30,Designer" >> employees.csv
myproject $ echo "Bob,28,Developer" >> employees.csv

# Extract just the names (using cut if available)
myproject $ cut -d',' -f1 < employees.csv > names.txt
myproject $ cat names.txt
John
Jane
Bob
```

### Configuration File Creation

```bash
# Build configuration files
myproject $ echo "# JSH Configuration" > .jshrc
myproject $ echo "export PATH=/usr/bin" >> .jshrc
myproject $ echo "alias ll='ls -la'" >> .jshrc

# Verify configuration
myproject $ cat .jshrc
# JSH Configuration
export PATH=/usr/bin
alias ll='ls -la'
```

## Limitations in JSH

### What JSH Supports

- Single input redirection per command: `cmd < file`
- Single output redirection per command: `cmd > file` or `cmd >> file`
- Combining input and output: `cmd < input > output`

### What JSH Doesn't Support

- **stderr redirection**: `2>` or `2>>`
- **Multiple redirections**: `cmd > file1 > file2`
- **Here documents**: `cmd << EOF`
- **Process substitution**: `cmd < <(other_cmd)`

## Practice Exercises

### Exercise 1: Basic Redirection

1. Create a file called `test.txt` with some content using `echo` and `>`
2. View the file using `cat`
3. Append more content using `>>`
4. View the updated file

### Exercise 2: Input Redirection

1. Create a file with several lines of text
2. Use `wc -l < filename` to count lines
3. Use `sort < filename` to sort the content
4. Save the sorted content to a new file

### Exercise 3: Combined Redirection

1. Create a file with unsorted data
2. Sort the file and save to a new file using: `sort < input.txt > output.txt`
3. Verify both files exist and compare their contents

### Exercise 4: Practical Workflow

1. Create a directory listing: `ls > files.txt`
2. Count the files: `wc -l < files.txt`
3. Add a timestamp: `date >> files.txt`
4. View the final result

## Troubleshooting

### Common Issues

**"Error: cannot open file"**
- File doesn't exist (for input redirection)
- No permission to write (for output redirection)
- Trying to redirect to a directory

**"Error: invalid command"**
- Missing filename after redirection operator
- Invalid syntax (multiple operators, etc.)

**Empty files created**
- Normal if command produces no output
- Check if command ran successfully first

**Accidentally overwriting files**
- Use `>>` instead of `>` to append
- Check file contents before redirecting

## Summary

In this chapter, you learned:

- ✅ How to redirect output with `>` (overwrite) and `>>` (append)
- ✅ How to redirect input with `<`
- ✅ How to combine input and output redirection
- ✅ Practical workflows for file processing
- ✅ Error handling and troubleshooting
- ✅ Limitations of JSH's redirection support

## Next Steps

Now that you understand I/O redirection, you're ready for Chapter 4, where you'll learn about pipelines - connecting commands together so the output of one becomes the input of another.

---

**Previous Chapter**: [← Basic Commands](02-basic-commands.md)  
**Next Chapter**: [Pipelines →](04-pipelines.md)