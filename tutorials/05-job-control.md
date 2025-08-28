# Chapter 5: Job Control

Job control is a powerful feature that allows you to manage multiple processes, suspend running commands, and resume them later. This is essential for multitasking in a shell environment.

## Understanding Jobs and Processes

### What is a Job?

In JSH, a **job** is a command or pipeline that can be:

- Running in the foreground
- Suspended (stopped)
- Running in the background (not supported in current JSH)

### Process vs. Job

- **Process**: A single running program with a unique Process ID (PID)
- **Job**: A collection of processes that can be managed as a unit
- In JSH, jobs are primarily pipelines that have been suspended

## Job States

JSH tracks jobs in three states:

| State | Description | How it Happens |
|-------|-------------|----------------|
| `JOB_RUNNING` | Currently executing | Default state when job starts |
| `JOB_STOPPED` | Suspended by signal | User presses Ctrl+Z |
| `JOB_TERMINATED` | Finished execution | Job completes or is killed |

## Signal Handling in JSH

### Key Signals

JSH handles these important signals:

| Signal | Key Combo | Effect | JSH Behavior |
|--------|-----------|--------|--------------|
| `SIGINT` | Ctrl+C | Interrupt | Ignored by shell, sent to foreground job |
| `SIGQUIT` | Ctrl+\ | Quit | Ignored by shell, sent to foreground job |
| `SIGTSTP` | Ctrl+Z | Stop | Suspends foreground job, adds to job list |

### How JSH Handles Signals

```c
// JSH ignores these signals for the shell process itself
signal(SIGINT, signal_handler);   // Ctrl+C
signal(SIGQUIT, signal_handler);  // Ctrl+\  
signal(SIGTSTP, signal_handler);  // Ctrl+Z
```

This means:

- Ctrl+C doesn't exit JSH itself
- Ctrl+Z in JSH doesn't suspend the shell
- These signals are passed to child processes instead

## Suspending Jobs (Ctrl+Z)

### Basic Job Suspension

Start a long-running command and suspend it:

```bash
# Start a long-running command
myproject $ sleep 60
# Press Ctrl+Z to suspend
^Z
myproject $ 
```

The `^Z` indicates the job was suspended and JSH returns to the prompt.

### What Happens When You Suspend

1. JSH sends `SIGTSTP` to the foreground process
2. The process stops execution
3. JSH adds the job to its job list
4. Control returns to the shell prompt
5. The job remains in memory but doesn't execute

### Suspending Different Types of Commands

```bash
# Suspend simple commands
myproject $ cat largefile.txt
^Z
myproject $ 

# Suspend pipelines
myproject $ grep -r "pattern" /large/directory | sort
^Z
myproject $ 

# Suspend interactive programs
myproject $ vi filename.txt
^Z
myproject $ 
```

## Managing Jobs with `jobs`

### Listing Jobs

The `jobs` command shows all suspended jobs:

```bash
myproject $ jobs
[1] sleep 60
[2] grep -r "pattern" /large/directory | sort
```

### Job Numbering

- Jobs are numbered sequentially starting from 1
- Numbers are reused when jobs complete
- The most recently suspended job doesn't always have the highest number

### Empty Job List

When no jobs are suspended:

```bash
myproject $ jobs
# (no output - job list is empty)
```

## Resuming Jobs with `fg`

### Basic Job Resumption

Bring a job back to the foreground:

```bash
# List jobs to see job numbers
myproject $ jobs
[1] sleep 60
[2] cat largefile.txt

# Resume job 1
myproject $ fg 1
sleep 60
# (continues running until completion)
```

### What Happens During Resume

1. JSH removes job from the suspended list
2. Sends `SIGCONT` to the process
3. Process continues from where it stopped
4. Job runs in foreground until completion or suspension

### Job Completion After Resume

When a resumed job completes:

```bash
myproject $ fg 1
sleep 60
# (after 60 seconds, returns to prompt)
myproject $ 

# Job is automatically removed from job list
myproject $ jobs
[2] cat largefile.txt
```

## Job Control Workflows

### Multitasking Workflow

```bash
# Start first task
myproject $ find / -name "*.log" > findresults.txt
^Z

# Start second task
myproject $ sort largefile.txt > sorted.txt
^Z

# Check what's suspended
myproject $ jobs
[1] find / -name "*.log" > findresults.txt
[2] sort largefile.txt > sorted.txt

# Resume first task
myproject $ fg 1
# (find completes)

# Check remaining jobs
myproject $ jobs
[1] sort largefile.txt > sorted.txt

# Resume second task
myproject $ fg 1
# (sort completes)
```

### Interactive Program Management

```bash
# Edit a file
myproject $ vi config.txt
^Z

# Do something else
myproject $ ls -la

# Return to editing
myproject $ fg 1
# (vi resumes with file still open)
```

### Long-Running Process Management

```bash
# Start a compilation or large operation
myproject $ gcc -g -Wall *.c -o myprogram
^Z

# Check system resources or do other work
myproject $ ps aux | grep gcc

# Resume compilation
myproject $ fg 1
# (compilation continues)
```

## Advanced Job Control Concepts

### Process Groups

When you suspend a pipeline, JSH manages all processes in that pipeline:

```bash
# This creates multiple processes
myproject $ cat huge.txt | grep "pattern" | sort | uniq
^Z

# All processes in the pipeline are suspended together
myproject $ jobs
[1] cat huge.txt | grep "pattern" | sort | uniq
```

### Job Memory Management

JSH properly manages memory for suspended jobs:

```c
// Job structure in JSH
typedef struct {
    pid_t pid;          // Process ID of last process in pipeline
    char *cmd;          // Command string for display
    job_status_t status; // JOB_STOPPED, JOB_RUNNING, etc.
} job_t;
```

### Signal Propagation

When you resume a job, signals are properly handled:

```bash
# Resume job
myproject $ fg 1
long_running_command

# Ctrl+C now affects the resumed job, not JSH
^C
myproject $ 
```

## Limitations and Special Cases

### What JSH Supports

- Suspending single commands and pipelines
- Listing suspended jobs with `jobs`
- Resuming jobs with `fg`
- Proper signal handling
- Memory cleanup when jobs complete

### What JSH Doesn't Support

- **Background execution**: No `&` operator or `bg` command
- **Job notifications**: No alerts when background jobs complete
- **Multiple job resume**: Can't resume multiple jobs simultaneously
- **Job killing**: No `kill` command for jobs

### Built-in Commands and Jobs

Built-in commands cannot be suspended because they execute in the shell process:

```bash
# This works - external command
myproject $ sleep 60
^Z

# This doesn't work - builtin command  
myproject $ cd /tmp
# (executes immediately, cannot be suspended)
```

### Exit Restrictions

JSH prevents exit when jobs are suspended:

```bash
myproject $ sleep 100
^Z
myproject $ exit
Error: there are suspended jobs

# Must handle jobs first
myproject $ jobs
[1] sleep 100
myproject $ fg 1
# (let sleep complete or Ctrl+C it)
myproject $ exit
```

## Troubleshooting Job Control

### Common Issues

**"Error: invalid job"**

```bash
# Job number doesn't exist
myproject $ fg 999
Error: invalid job

# Check valid job numbers
myproject $ jobs
[1] sleep 100
myproject $ fg 1  # This works
```

**Jobs "disappear" from list**

- Normal when jobs complete
- Memory cleanup removes finished jobs
- Use `jobs` to check current status

**Can't exit shell**

```bash
myproject $ exit
Error: there are suspended jobs

# Solution: resume and complete jobs
myproject $ jobs
myproject $ fg 1
# Handle each job, then exit
```

### Debugging Job Issues

Check job status regularly:

```bash
# Before suspending
myproject $ ps aux | grep mycommand

# After suspending  
myproject $ jobs
myproject $ ps aux | grep mycommand  # Process still exists

# After resuming
myproject $ ps aux | grep mycommand  # Process continues
```

## Practice Exercises

### Exercise 1: Basic Job Control

1. Start JSH
2. Run `sleep 30`
3. Suspend it with Ctrl+Z
4. Use `jobs` to list it
5. Resume with `fg 1`
6. Let it complete

### Exercise 2: Multiple Jobs

1. Start and suspend `sleep 60`
2. Start and suspend `cat /etc/passwd`
3. Use `jobs` to list both
4. Resume job 2, then job 1
5. Verify job list is empty

### Exercise 3: Pipeline Jobs

1. Create a large file: `ls -laR / > largefile.txt` (then Ctrl+C)
2. Start `cat largefile.txt | sort | head -20`
3. Suspend the pipeline
4. Check with `jobs`
5. Resume and let it complete

### Exercise 4: Real-world Scenario

1. Start editing a file with `vi` or `nano`
2. Suspend the editor
3. Run some commands to gather information
4. Resume the editor
5. Continue editing with the gathered information

## Best Practices

### When to Use Job Control

- **Long-running tasks**: Compilations, large file operations
- **Interactive programs**: Editors, when you need to check something
- **System monitoring**: When running monitoring commands
- **Development workflow**: Switch between editing and testing

### When Not to Use Job Control  

- **Short commands**: No point suspending quick operations
- **Critical operations**: Don't suspend important system operations
- **Built-in commands**: They can't be suspended anyway

### Memory Considerations

- Each suspended job uses memory
- JSH limits jobs to `MAX_JOBS` (100)
- Clean up completed jobs automatically
- Don't accumulate too many suspended jobs

## Summary

In this chapter, you learned:

- ✅ How job control works in JSH
- ✅ How to suspend jobs with Ctrl+Z
- ✅ How to list jobs with the `jobs` command
- ✅ How to resume jobs with the `fg` command
- ✅ Signal handling and process management
- ✅ Limitations and best practices
- ✅ Troubleshooting common job control issues

Job control is essential for efficient multitasking in a Unix shell environment, allowing you to manage multiple tasks seamlessly.

## Next Steps

Now that you understand job control, you're ready for Chapter 6, where you'll learn advanced usage patterns that combine all the features you've learned: commands, redirection, pipelines, and job control.

---

**Previous Chapter**: [← Pipelines](04-pipelines.md)  
**Next Chapter**: [Advanced Usage →](06-advanced-usage.md)

