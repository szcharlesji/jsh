# Chapter 7: Architecture

This chapter explores JSH's internal architecture, explaining how the code implements the features you've been using. Understanding the architecture will help you extend JSH or build your own shell.

## Overview of JSH Architecture

JSH follows a modular design with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────┐
│                        main.c                           │
│                   Main shell loop                       │
└─────────────────┬───────────────────────┬───────────────┘
                  │                       │
                  ▼                       ▼
      ┌─────────────────────┐  ┌─────────────────────┐
      │     parser.c        │  │     executor.c      │
      │ Command parsing     │  │ Command execution   │
      │ Built-in commands   │  │ Pipeline handling   │
      └─────────────────────┘  └─────────────────────┘
                  │                       │
                  └───────────┬───────────┘
                              ▼
                  ┌─────────────────────┐
                  │       jobs.c        │
                  │   Job control       │
                  │  Signal handling    │
                  └─────────────────────┘
```

## Core Data Structures

### Command Structure (`command_t`)

Located in `jsh.h:31-36`, represents a single command:

```c
typedef struct {
    char **args;                // Command arguments (NULL-terminated)
    int argc;                   // Number of arguments
    operator_t redirect_type;   // Type of redirection (>, >>, <, or none)
    char *redirect_file;        // Target file for redirection
} command_t;
```

**Usage Examples:**
- `ls -la` → `args = ["ls", "-la", NULL]`, `argc = 2`, `redirect_type = OP_NONE`
- `cat > file.txt` → `args = ["cat", NULL]`, `redirect_type = OP_REDIRECT_OUT`, `redirect_file = "file.txt"`

### Pipeline Structure (`pipeline_t`)

Located in `jsh.h:38-42`, represents a complete command line:

```c
typedef struct {
    command_t *commands;    // Array of commands in the pipeline
    int num_commands;       // Number of commands in pipeline
} pipeline_t;
```

**Usage Examples:**
- `ls` → `num_commands = 1`, `commands[0]` contains the ls command
- `ls | wc -l` → `num_commands = 2`, `commands[0]` = ls, `commands[1]` = wc

### Job Structure (`job_t`)

Located in `jobs.h:14-18`, represents a suspended job:

```c
typedef struct {
    pid_t pid;              // Process ID of the last process in pipeline
    char *cmd;              // Command string for display purposes
    job_status_t status;    // Job status (RUNNING, STOPPED, TERMINATED)
} job_t;
```

## Main Shell Loop (main.c)

### Program Flow

The main shell loop in `main.c:6-82` follows this pattern:

```c
int main(void) {
    setup_signal_handlers();           // Initialize signal handling
    
    while (flag) {
        display_prompt();              // Show directory prompt
        read_command();                // Get user input
        parse_command();               // Convert to pipeline structure
        execute_command();             // Run the command/pipeline
        cleanup_memory();              // Free allocated resources
    }
    
    cleanup_jobs();                    // Clean up before exit
    return 0;
}
```

### Key Implementation Details

**Prompt Generation (`main.c:16-18`):**
```c
getcwd(wd, sizeof(wd));               // Get current working directory
base = basename(wd);                  // Extract directory name
printf("%s $ ", base);                // Display prompt
```

**Command Reading (`main.c:22-32`):**
```c
if (fgets(command, MAX_LEN, stdin) == NULL) {
    printf("\n");
    flag = false;                     // EOF received, exit shell
    break;
}
// Remove trailing newline
if (len > 0 && command[len - 1] == '\n') {
    command[len - 1] = '\0';
}
```

**Built-in Detection (`main.c:64-69`):**
```c
if (pipeline->num_commands == 1 &&
    is_builtin(pipeline->commands[0].args[0])) {
    int result = execute_builtin(pipeline->commands[0].args);
    // Handle exit command specially
}
```

## Command Parsing (parser.c)

### Parsing Process

The parser converts raw command strings into structured data:

```
"ls -la | grep txt > results.txt"
            ↓
    Tokenization
            ↓
    ["ls", "-la", "|", "grep", "txt", ">", "results.txt"]
            ↓
    Syntax Validation
            ↓
    Structure Creation
            ↓
    pipeline_t with 2 commands:
    - commands[0]: ls -la
    - commands[1]: grep txt (with output redirect)
```

### Key Functions

**`parse_command_line()` (`parser.c:85-177`)**

1. **Tokenization** (`parser.c:94-98`):
   ```c
   args[0] = strtok(command, " \n");
   while (args[num_args] != NULL && num_args < MAX_ARGS - 1) {
       num_args++;
       args[num_args] = strtok(NULL, " \n");
   }
   ```

2. **Syntax Validation** (`parser.c:106-109`):
   ```c
   if (validate_command_syntax(args, num_args) != 0) {
       free(pipeline);
       return NULL;
   }
   ```

3. **Pipeline Structure Creation** (`parser.c:119-124`):
   ```c
   // Count pipes to determine number of commands
   int pipe_count = 0;
   for (int i = 0; i < num_args; i++) {
       if (args[i] && strcmp(args[i], "|") == 0) {
           pipe_count++;
       }
   }
   pipeline->num_commands = pipe_count + 1;
   ```

**`get_operator_type()` (`parser.c:73-83`)**

Identifies redirection and pipe operators:
```c
operator_t get_operator_type(const char *str) {
    if (strcmp(str, "|") == 0)  return OP_PIPE;
    if (strcmp(str, ">") == 0)  return OP_REDIRECT_OUT;
    if (strcmp(str, ">>") == 0) return OP_REDIRECT_APPEND;
    if (strcmp(str, "<") == 0)  return OP_REDIRECT_IN;
    return OP_NONE;
}
```

### Built-in Command Handling

**Built-in Detection (`parser.c:4-7`)**:
```c
bool is_builtin(const char *cmd) {
    return (strcmp(cmd, "exit") == 0 || strcmp(cmd, "cd") == 0 ||
            strcmp(cmd, "jobs") == 0 || strcmp(cmd, "fg") == 0);
}
```

**Built-in Execution (`parser.c:9-54`)**:
Each built-in is handled with specific logic:
- `exit`: Checks for suspended jobs before allowing exit
- `cd`: Changes directory and handles errors  
- `jobs`: Lists suspended jobs from global job array
- `fg`: Resumes specified job and manages job list

## Command Execution (executor.c)

### Execution Flow

The executor handles both single commands and pipelines:

```
Pipeline received
       ↓
Single command? ──Yes──→ execute_single_command()
       ↓ No                        ↓
Create pipes ←────────────────── Built-in? ──Yes──→ execute_builtin()
       ↓                           ↓ No
Fork processes                   Fork process
       ↓                           ↓
Connect pipes                  Setup redirection  
       ↓                           ↓
Execute commands                  exec() system call
       ↓
Wait for completion
       ↓
Add jobs if suspended
```

### Key Functions

**`execute_pipeline()` (`executor.c:118-223`)**

For single commands, delegates to `execute_single_command()`. For pipelines:

1. **Create Pipes** (`executor.c:134-139`):
   ```c
   int pipes[num_pipes][2];
   for (int i = 0; i < num_pipes; i++) {
       if (pipe(pipes[i]) == -1) {
           fprintf(stderr, "Error: pipe failed\n");
           return -1;
       }
   }
   ```

2. **Fork Processes** (`executor.c:156-200`):
   ```c
   for (int i = 0; i < pipeline->num_commands; i++) {
       pids[i] = fork();
       if (pids[i] == 0) {
           // Child process: setup pipes and exec
       }
   }
   ```

3. **Setup Process Connections** (`executor.c:161-174`):
   ```c
   // Connect input from previous pipe
   if (i > 0) {
       dup2(pipes[i - 1][0], STDIN_FILENO);
   }
   
   // Connect output to next pipe  
   if (i < pipeline->num_commands - 1) {
       dup2(pipes[i][1], STDOUT_FILENO);
   }
   ```

**`setup_redirection()` (`executor.c:26-77`)**

Handles I/O redirection for individual commands:
```c
switch (cmd->redirect_type) {
    case OP_REDIRECT_OUT:
        fd = open(cmd->redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
        break;
    case OP_REDIRECT_APPEND:
        fd = open(cmd->redirect_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        dup2(fd, STDOUT_FILENO);  
        break;
    case OP_REDIRECT_IN:
        fd = open(cmd->redirect_file, O_RDONLY);
        dup2(fd, STDIN_FILENO);
        break;
}
```

## Job Control System (jobs.c)

### Job Management

JSH maintains a global job array and counter:

```c
job_t jobs[MAX_JOBS];    // Global job array  
int num_jobs = 0;        // Current number of jobs
```

### Signal Handling

**Signal Handler Setup (`jobs.c:12-16`)**:
```c
void setup_signal_handlers(void) {
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGQUIT, signal_handler);  // Ctrl+\
    signal(SIGTSTP, signal_handler);  // Ctrl+Z
}
```

**Signal Handler Implementation (`jobs.c:6-10`)**:
```c
void signal_handler(int sig) {
    // Empty handler - signals are ignored by shell
    // but passed to child processes
    (void)sig; // Suppress unused parameter warning
}
```

### Job Operations

**Adding Jobs (`jobs.c:18-37`)**:
```c
int add_job(int status, pid_t pid, char *cmdStr) {
    if (WIFSTOPPED(status)) {        // Only add if process was stopped
        if (num_jobs < MAX_JOBS) {
            jobs[num_jobs].cmd = malloc(strlen(cmdStr) + 1);
            strcpy(jobs[num_jobs].cmd, cmdStr);
            jobs[num_jobs].pid = pid;
            jobs[num_jobs].status = JOB_STOPPED;
            num_jobs++;
            return 0;
        }
    }
    return 0;
}
```

**Resuming Jobs (`jobs.c:61-89`)**:
```c
int fg_job(int job_num) {
    int job_index = job_num - 1;
    pid_t pid = jobs[job_index].pid;
    
    // Remove job from array before continuing
    remove_job(job_index);
    
    // Continue the job and wait for it
    kill(pid, SIGCONT);              // Send continue signal
    waitpid(pid, &status, WUNTRACED); // Wait for completion or stop
    
    // If job was stopped again, add it back
    add_job(status, pid, cmdName);
}
```

## Memory Management

### Allocation Strategy

JSH uses careful memory management to prevent leaks:

**Pipeline Allocation (`parser.c:86-124`)**:
```c
pipeline_t *pipeline = malloc(sizeof(pipeline_t));
pipeline->commands = malloc(pipeline->num_commands * sizeof(command_t));

// For each command:
cmd->args = malloc((cmd_argc + 1) * sizeof(char *));
cmd->args[arg_idx] = strdup(args[j]);  // Duplicate strings
cmd->redirect_file = strdup(args[j + 1]); // Duplicate filenames
```

**Cleanup Functions (`parser.c:179-207`)**:
```c
void free_command(command_t *cmd) {
    if (cmd->args) {
        for (int i = 0; i < cmd->argc; i++) {
            free(cmd->args[i]);          // Free each argument string
        }
        free(cmd->args);                 // Free argument array
    }
    if (cmd->redirect_file) {
        free(cmd->redirect_file);        // Free redirection filename
    }
}

void free_pipeline(pipeline_t *pipeline) {
    if (pipeline->commands) {
        for (int i = 0; i < pipeline->num_commands; i++) {
            free_command(&pipeline->commands[i]); // Free each command
        }
        free(pipeline->commands);        // Free command array
    }
    free(pipeline);                      // Free pipeline structure
}
```

### Job Memory Management

Jobs allocate memory for command strings:
```c
// Allocation
jobs[num_jobs].cmd = malloc(strlen(cmdStr) + 1);
strcpy(jobs[num_jobs].cmd, cmdStr);

// Cleanup  
void cleanup_jobs(void) {
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].cmd) {
            free(jobs[i].cmd);
        }
    }
    num_jobs = 0;
}
```

## Error Handling

### Error Reporting Strategy

JSH uses consistent error messages across modules:

**Command Errors**:
- `"Error: invalid command"` - Syntax errors, built-in misuse
- `"Error: invalid program"` - External command not found  
- `"Error: invalid directory"` - `cd` to non-existent directory

**File Errors**:
- `"Error: cannot open file %s"` - I/O redirection failures

**Job Errors**:  
- `"Error: invalid job"` - Invalid job number for `fg`
- `"Error: there are suspended jobs"` - Exit with active jobs

### Error Handling Patterns

**Validation Before Execution**:
```c
// Syntax validation
if (validate_command_syntax(args, num_args) != 0) {
    free(pipeline);
    return NULL;
}

// External command validation  
if (check_external_command(cmd->args[0]) != 0) {
    return -1;
}
```

**Resource Cleanup on Error**:
```c
if (pipe(pipes[i]) == -1) {
    fprintf(stderr, "Error: pipe failed\n");
    // Clean up already created pipes
    for (int j = 0; j < i; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
    return -1;
}
```

## Limitations and Design Decisions

### Current Limitations

1. **Command Path**: Only checks `/usr/bin/` for external commands
2. **Built-in Pipelines**: Built-ins cannot be used in pipelines
3. **Background Jobs**: No `&` operator or `bg` command
4. **Environment Variables**: No variable expansion or `export`
5. **Advanced Redirection**: No stderr redirection or here documents

### Design Rationale

**Simplicity Over Features**: JSH prioritizes educational clarity over comprehensive functionality.

**POSIX Compliance**: Uses standard POSIX system calls (`fork`, `exec`, `pipe`, `dup2`).

**Memory Safety**: Careful allocation and cleanup prevents memory leaks.

**Error Handling**: Clear, consistent error messages help users understand issues.

## Extending JSH

### Adding New Built-in Commands

1. **Update `is_builtin()`** in `parser.c:4-7`
2. **Add case to `execute_builtin()`** in `parser.c:9-54`  
3. **Implement the command logic**
4. **Update help text and documentation**

### Adding New Features

**Environment Variables**:
- Add variable storage structure
- Implement variable expansion in parser
- Add `export`/`unset` built-ins

**Background Jobs**:
- Modify executor to handle `&` operator
- Implement `bg` command
- Add job status notification

**Advanced Redirection**:
- Extend `operator_t` enum
- Update parser to recognize new operators
- Implement in `setup_redirection()`

## Performance Characteristics

### Time Complexity

- **Parsing**: O(n) where n is command length
- **Pipeline Execution**: O(p) where p is number of processes  
- **Job Management**: O(j) where j is number of jobs

### Space Complexity

- **Command Storage**: O(n) for command strings and arguments
- **Job Storage**: O(j) for suspended jobs (max MAX_JOBS = 100)
- **Pipeline Storage**: O(p) for pipeline processes

### System Resource Usage

- **File Descriptors**: 2 per pipe in pipeline
- **Memory**: Minimal heap allocation, careful cleanup
- **Processes**: 1 per command in pipeline

## Summary

JSH's architecture demonstrates:

- ✅ **Modular Design**: Clear separation between parsing, execution, and job control
- ✅ **Standard Practices**: Uses established Unix system calls and patterns  
- ✅ **Memory Safety**: Careful allocation and cleanup prevents leaks
- ✅ **Error Handling**: Consistent error reporting across modules
- ✅ **Extensibility**: Clean interfaces allow adding new features
- ✅ **Educational Value**: Code structure teaches shell implementation concepts

Understanding this architecture prepares you to:
- Extend JSH with new features
- Debug issues in the implementation  
- Build your own shell from scratch
- Appreciate the complexity of production shells like bash

The modular design makes JSH an excellent foundation for learning systems programming and understanding how shells work at a fundamental level.

---

**Previous Chapter**: [← Advanced Usage](06-advanced-usage.md)  
**Tutorial Complete!** 🎉

You now have comprehensive knowledge of JSH's features and implementation. Consider exploring the source code directly to deepen your understanding of shell internals.