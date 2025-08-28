# Chapter 6: Advanced Usage

Now that you understand all the core features of JSH, this chapter demonstrates how to combine them effectively for complex workflows. You'll learn practical patterns and real-world scenarios that showcase JSH's capabilities.

## Combining All Features

JSH becomes powerful when you combine its features:
- **Built-in commands** for shell control
- **I/O redirection** for file operations  
- **Pipelines** for data processing
- **Job control** for multitasking

## Complex Pipeline Workflows

### Data Analysis Pipeline

Create a comprehensive data analysis workflow:

```bash
# Create sample data file
myproject $ echo "user1,login,2023-01-15,10:30" > access.log
myproject $ echo "user2,logout,2023-01-15,10:32" >> access.log
myproject $ echo "user1,error,2023-01-15,10:35" >> access.log
myproject $ echo "user3,login,2023-01-15,10:40" >> access.log
myproject $ echo "user2,login,2023-01-15,10:45" >> access.log

# Extract login events and analyze
myproject $ cat access.log | grep "login" | cut -d',' -f1 | sort | uniq -c > login_summary.txt

# View the results
myproject $ cat login_summary.txt
   2 user1
   1 user2
   1 user3
```

### Multi-Stage Processing

Process data through multiple stages:

```bash
# Stage 1: Extract and filter
myproject $ ls -la | grep "^-" > files_only.txt

# Stage 2: Process file info
myproject $ cat files_only.txt | grep "\.c$" | wc -l > source_count.txt

# Stage 3: Combine results
myproject $ echo "C source files: $(cat source_count.txt)" > project_stats.txt
myproject $ ls *.h | wc -l >> project_stats.txt

# View final results
myproject $ cat project_stats.txt
```

### Log Analysis Workflow

Analyze system or application logs:

```bash
# Create sample log
myproject $ echo "2023-01-15 10:30:15 [INFO] System started" > system.log
myproject $ echo "2023-01-15 10:30:20 [ERROR] Database connection failed" >> system.log
myproject $ echo "2023-01-15 10:30:25 [INFO] Retrying connection" >> system.log
myproject $ echo "2023-01-15 10:30:30 [INFO] Connection established" >> system.log
myproject $ echo "2023-01-15 10:30:35 [ERROR] User authentication failed" >> system.log

# Extract error information
myproject $ cat system.log | grep "ERROR" | wc -l > error_count.txt
myproject $ cat system.log | grep "ERROR" > errors_detail.txt

# Generate summary report
myproject $ echo "=== System Log Analysis ===" > report.txt
myproject $ echo "Total errors: $(cat error_count.txt)" >> report.txt
myproject $ echo "" >> report.txt
myproject $ echo "Error details:" >> report.txt
myproject $ cat errors_detail.txt >> report.txt
```

## Advanced Job Control Patterns

### Development Workflow

Manage multiple development tasks simultaneously:

```bash
# Start a long compilation
myproject $ gcc -g -Wall *.c -o jsh
^Z

# Edit source while compilation is suspended
myproject $ jobs
[1] gcc -g -Wall *.c -o jsh

# Run quick tests
myproject $ echo "Testing basic functionality" > test.log
myproject $ ./jsh < /dev/null >> test.log 2>&1

# Resume compilation
myproject $ fg 1
# (compilation continues)

# Check results
myproject $ ls -la jsh
```

### Monitoring and Debugging

Monitor system resources while debugging:

```bash
# Start monitoring in background (simulate with log file)
myproject $ while true; do date >> system_monitor.log; sleep 5; done
^Z

# Debug your application
myproject $ ./jsh
# (test your shell)
myproject $ exit

# Check monitoring data
myproject $ jobs
[1] while true; do date >> system_monitor.log; sleep 5; done

# Stop monitoring
myproject $ fg 1
^C
```

### Interactive Session Management

Manage multiple interactive tasks:

```bash
# Start editing configuration
myproject $ vi config.txt
^Z

# Check current directory and files
myproject $ pwd
myproject $ ls -la

# Run tests to inform your editing
myproject $ cat test_data.txt | grep "config" > config_tests.txt

# Resume editing with new information
myproject $ fg 1
# (continue editing with test results in mind)
```

## File Processing Workflows

### Batch File Processing

Process multiple files systematically:

```bash
# Create multiple source files for testing
myproject $ echo "#include <stdio.h>" > test1.c
myproject $ echo "#include <stdlib.h>" >> test1.c
myproject $ echo "#include <stdio.h>" > test2.c
myproject $ echo "#include <string.h>" >> test2.c

# Process all C files
myproject $ ls *.c > c_files.txt
myproject $ cat c_files.txt | wc -l > file_count.txt
myproject $ echo "Processing $(cat file_count.txt) C files" > process_log.txt

# Analyze includes in all files
myproject $ cat *.c | grep "#include" | sort | uniq > unique_includes.txt
myproject $ cat unique_includes.txt | wc -l >> process_log.txt
myproject $ echo "unique includes found" >> process_log.txt
```

### Configuration Management

Manage configuration files effectively:

```bash
# Create base configuration
myproject $ echo "# JSH Configuration" > .jshrc
myproject $ echo "PATH=/usr/bin:/bin" >> .jshrc
myproject $ echo "EDITOR=vi" >> .jshrc

# Create user-specific additions
myproject $ echo "" > user_config.txt
myproject $ echo "# User customizations" >> user_config.txt
myproject $ echo "CUSTOM_VAR=value" >> user_config.txt

# Combine configurations
myproject $ cat .jshrc > final_config.txt
myproject $ cat user_config.txt >> final_config.txt

# Validate configuration
myproject $ cat final_config.txt | grep -v "^#" | grep "=" > active_settings.txt
```

## Data Transformation Pipelines

### Text Processing Chains

Transform text data through multiple steps:

```bash
# Create sample text data
myproject $ echo "apple,red,sweet" > fruits.csv
myproject $ echo "banana,yellow,sweet" >> fruits.csv
myproject $ echo "lemon,yellow,sour" >> fruits.csv
myproject $ echo "apple,green,tart" >> fruits.csv

# Multi-stage processing
myproject $ cat fruits.csv | cut -d',' -f2 | sort | uniq -c > color_count.txt
myproject $ cat fruits.csv | cut -d',' -f3 | sort | uniq -c > taste_count.txt

# Generate summary
myproject $ echo "=== Fruit Analysis ===" > analysis.txt
myproject $ echo "Color distribution:" >> analysis.txt
myproject $ cat color_count.txt >> analysis.txt
myproject $ echo "" >> analysis.txt
myproject $ echo "Taste distribution:" >> analysis.txt
myproject $ cat taste_count.txt >> analysis.txt
```

### Format Conversion

Convert data between formats:

```bash
# Create CSV data
myproject $ echo "name,age,role" > employees.csv
myproject $ echo "John,25,Developer" >> employees.csv
myproject $ echo "Jane,30,Designer" >> employees.csv
myproject $ echo "Bob,35,Manager" >> employees.csv

# Convert to different formats
myproject $ cat employees.csv | grep -v "name,age,role" > data_only.csv
myproject $ cat data_only.csv | cut -d',' -f1 > names.txt
myproject $ cat data_only.csv | cut -d',' -f3 > roles.txt

# Create formatted report
myproject $ echo "Employee Report" > report.txt
myproject $ echo "===============" >> report.txt
myproject $ paste names.txt roles.txt | sed 's/\t/ - /' >> report.txt
```

## System Administration Tasks

### File System Analysis

Analyze file system usage and organization:

```bash
# Analyze current directory
myproject $ ls -la > dir_listing.txt
myproject $ cat dir_listing.txt | grep "^-" | wc -l > file_count.txt
myproject $ cat dir_listing.txt | grep "^d" | wc -l > dir_count.txt

# Create summary
myproject $ echo "Directory Analysis" > dir_analysis.txt
myproject $ echo "=================" >> dir_analysis.txt
myproject $ echo "Files: $(cat file_count.txt)" >> dir_analysis.txt
myproject $ echo "Directories: $(cat dir_count.txt)" >> dir_analysis.txt
myproject $ echo "" >> dir_analysis.txt
myproject $ echo "File types:" >> dir_analysis.txt
myproject $ ls | grep -o "\.[^.]*$" | sort | uniq -c >> dir_analysis.txt
```

### Log Maintenance

Maintain and analyze log files:

```bash
# Create sample application log
myproject $ echo "$(date): Application started" > app.log
myproject $ echo "$(date): Processing user request" >> app.log
myproject $ echo "$(date): Database query executed" >> app.log
myproject $ echo "$(date): Warning: High memory usage" >> app.log
myproject $ echo "$(date): Error: Connection timeout" >> app.log

# Extract important events
myproject $ cat app.log | grep -i "error\|warning" > alerts.log
myproject $ cat app.log | wc -l > total_entries.txt
myproject $ cat alerts.log | wc -l > alert_count.txt

# Generate maintenance report
myproject $ echo "Log Maintenance Report" > maint_report.txt
myproject $ echo "=====================" >> maint_report.txt
myproject $ echo "Total log entries: $(cat total_entries.txt)" >> maint_report.txt
myproject $ echo "Alerts generated: $(cat alert_count.txt)" >> maint_report.txt
myproject $ echo "" >> maint_report.txt
myproject $ echo "Recent alerts:" >> maint_report.txt
myproject $ cat alerts.log >> maint_report.txt
```

## Development Workflows

### Code Analysis

Analyze source code structure:

```bash
# Analyze JSH source code
myproject $ ls *.c *.h > all_sources.txt
myproject $ cat *.c | grep "#include" | sort | uniq > includes.txt
myproject $ cat *.c | grep "^[a-zA-Z].*(" | head -10 > functions.txt

# Generate code report
myproject $ echo "JSH Code Analysis" > code_report.txt
myproject $ echo "=================" >> code_report.txt
myproject $ echo "Source files: $(cat all_sources.txt | wc -l)" >> code_report.txt
myproject $ echo "Unique includes: $(cat includes.txt | wc -l)" >> code_report.txt
myproject $ echo "" >> code_report.txt
myproject $ echo "Common includes:" >> code_report.txt
myproject $ cat includes.txt >> code_report.txt
```

### Build Automation

Automate build and test processes:

```bash
# Clean previous builds
myproject $ make clean > build.log 2>&1

# Build project with logging
myproject $ make >> build.log 2>&1

# Check build results
myproject $ cat build.log | grep -i "error\|warning" > build_issues.txt

# Generate build report
myproject $ echo "Build Report" > build_report.txt
myproject $ echo "============" >> build_report.txt
myproject $ date >> build_report.txt
myproject $ echo "" >> build_report.txt
myproject $ if [ -s build_issues.txt ]; then
    echo "Issues found:" >> build_report.txt
    cat build_issues.txt >> build_report.txt
  else
    echo "Build completed successfully" >> build_report.txt
  fi
```

## Performance Monitoring

### Resource Usage Tracking

Monitor and track resource usage:

```bash
# Create resource monitoring script simulation
myproject $ echo "monitoring system resources..." > resource.log
myproject $ date >> resource.log
myproject $ ps aux | head -5 >> resource.log

# Long-running monitoring (suspend for other work)
myproject $ while true; do 
    echo "$(date): System check" >> monitor.log
    sleep 10
  done
^Z

# Do other work while monitoring
myproject $ jobs
[1] monitoring loop

# Check monitoring results
myproject $ tail -5 monitor.log > recent_checks.txt

# Resume or stop monitoring as needed
myproject $ fg 1
^C
```

## Error Handling and Debugging

### Robust Pipeline Construction

Build pipelines that handle errors gracefully:

```bash
# Create test data with potential issues
myproject $ echo "valid_data_1" > test_data.txt
myproject $ echo "" >> test_data.txt
myproject $ echo "valid_data_2" >> test_data.txt
myproject $ echo "   " >> test_data.txt
myproject $ echo "valid_data_3" >> test_data.txt

# Process with error checking
myproject $ cat test_data.txt | grep -v "^$" | grep -v "^[[:space:]]*$" > clean_data.txt
myproject $ cat clean_data.txt | wc -l > valid_count.txt

# Verify results
myproject $ echo "Data processing results:" > results.txt
myproject $ echo "Valid entries: $(cat valid_count.txt)" >> results.txt
myproject $ echo "Clean data:" >> results.txt
myproject $ cat clean_data.txt >> results.txt
```

### Debugging Complex Workflows

Debug multi-step processes systematically:

```bash
# Complex workflow with intermediate checking
myproject $ echo "Starting complex workflow..." > debug.log

# Step 1: Data preparation
myproject $ ls *.c > step1_output.txt
myproject $ echo "Step 1 complete: $(cat step1_output.txt | wc -l) files" >> debug.log

# Step 2: Processing  
myproject $ cat step1_output.txt | head -3 > step2_output.txt
myproject $ echo "Step 2 complete: $(cat step2_output.txt | wc -l) files selected" >> debug.log

# Step 3: Final processing
myproject $ cat step2_output.txt | sort > final_output.txt
myproject $ echo "Step 3 complete: final output created" >> debug.log

# Review debugging info
myproject $ echo "Debugging Summary" > debug_summary.txt
myproject $ echo "=================" >> debug_summary.txt
myproject $ cat debug.log >> debug_summary.txt
```

## Best Practices for Advanced Usage

### Workflow Organization

1. **Use descriptive filenames**: `analysis_results.txt`, `error_log.txt`
2. **Create intermediate files**: Save results at each step
3. **Use consistent naming**: Follow patterns like `step1_`, `step2_`
4. **Clean up temp files**: Remove intermediate files when done

### Job Management

1. **Monitor active jobs**: Regular `jobs` checks
2. **Don't accumulate jobs**: Complete or terminate suspended jobs
3. **Use meaningful commands**: Choose descriptive command names
4. **Plan interruptions**: Know when you can safely suspend

### Pipeline Design

1. **Test components individually**: Verify each command works
2. **Use incremental complexity**: Build pipelines step by step  
3. **Save intermediate results**: Capture important intermediate data
4. **Handle empty input**: Consider what happens with no data

## Real-World Scenarios

### Project Build Management

```bash
# Complete project build workflow
myproject $ echo "Starting build process..." > build_session.log
myproject $ date >> build_session.log

# Clean and build
myproject $ make clean > /dev/null 2>&1
myproject $ make > build_output.txt 2>&1

# Check for issues
myproject $ cat build_output.txt | grep -i "error\|warning" > build_issues.txt

# Generate final report
myproject $ if [ -s build_issues.txt ]; then
    echo "BUILD FAILED - Issues found:" >> build_session.log
    cat build_issues.txt >> build_session.log
  else
    echo "BUILD SUCCESSFUL" >> build_session.log
  fi

myproject $ echo "Build completed at $(date)" >> build_session.log
```

### System Maintenance

```bash
# System maintenance routine
myproject $ echo "=== System Maintenance $(date) ===" > maintenance.log

# Check disk usage simulation
myproject $ ls -la | head -10 > disk_usage.txt
myproject $ echo "Disk check completed" >> maintenance.log

# Check processes
myproject $ ps | head -10 > process_check.txt
myproject $ echo "Process check completed" >> maintenance.log

# Generate summary
myproject $ echo "" >> maintenance.log
myproject $ echo "Maintenance completed successfully" >> maintenance.log
myproject $ cat maintenance.log
```

## Summary

In this chapter, you learned how to:

- ✅ Combine all JSH features for complex workflows
- ✅ Create sophisticated data processing pipelines  
- ✅ Manage development and debugging workflows
- ✅ Handle system administration tasks
- ✅ Build robust, error-resistant processes
- ✅ Apply best practices for advanced shell usage

These advanced patterns demonstrate JSH's capabilities and prepare you for real-world shell usage scenarios.

## Next Steps

You're now ready for the final chapter, Chapter 7, where you'll learn about JSH's internal architecture and how the code implements all the features you've been using.

---

**Previous Chapter**: [← Job Control](05-job-control.md)  
**Next Chapter**: [Architecture →](07-architecture.md)