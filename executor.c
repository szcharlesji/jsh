#include "executor.h"
#include "jobs.h"

int check_external_command(const char *cmd) {
    if (!cmd) return -1;
    
    char *full_path = malloc(strlen(cmd) + strlen("/usr/bin/") + 1);
    if (!full_path) return -1;
    
    if (cmd[0] != '/' && cmd[0] != '.' && !strchr(cmd, '/')) {
        strcpy(full_path, "/usr/bin/");
        strcat(full_path, cmd);
        if (access(full_path, X_OK) != 0) {
            free(full_path);
            fprintf(stderr, "Error: invalid program\n");
            return -1;
        }
    }
    
    free(full_path);
    return 0;
}

int setup_redirection(command_t *cmd) {
    if (!cmd || cmd->redirect_type == OP_NONE) {
        return 0;
    }
    
    int fd;
    switch (cmd->redirect_type) {
        case OP_REDIRECT_OUT:
            fd = open(cmd->redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                fprintf(stderr, "Error: cannot open file %s\n", cmd->redirect_file);
                return -1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                close(fd);
                return -1;
            }
            close(fd);
            break;
            
        case OP_REDIRECT_APPEND:
            fd = open(cmd->redirect_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                fprintf(stderr, "Error: cannot open file %s\n", cmd->redirect_file);
                return -1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                close(fd);
                return -1;
            }
            close(fd);
            break;
            
        case OP_REDIRECT_IN:
            fd = open(cmd->redirect_file, O_RDONLY);
            if (fd == -1) {
                fprintf(stderr, "Error: cannot open file %s\n", cmd->redirect_file);
                return -1;
            }
            if (dup2(fd, STDIN_FILENO) == -1) {
                close(fd);
                return -1;
            }
            close(fd);
            break;
            
        default:
            break;
    }
    
    return 0;
}

int execute_single_command(command_t *cmd) {
    if (!cmd || !cmd->args || !cmd->args[0]) {
        return -1;
    }
    
    // Check if it's a builtin command
    if (is_builtin(cmd->args[0])) {
        return execute_builtin(cmd->args);
    }
    
    // Check if external command exists
    if (check_external_command(cmd->args[0]) != 0) {
        return -1;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (setup_redirection(cmd) != 0) {
            exit(1);
        }
        
        execvp(cmd->args[0], cmd->args);
        fprintf(stderr, "Error: invalid command\n");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, WUNTRACED);
        // Note: We don't add single commands to jobs - only pipelines can be suspended
        return WEXITSTATUS(status);
    } else {
        // Fork failed
        fprintf(stderr, "Fork error\n");
        return -1;
    }
}

int execute_pipeline(pipeline_t *pipeline, char *original_cmd) {
    if (!pipeline || pipeline->num_commands == 0) {
        return -1;
    }
    
    // Handle single command case
    if (pipeline->num_commands == 1) {
        return execute_single_command(&pipeline->commands[0]);
    }
    
    // Handle pipeline
    int num_pipes = pipeline->num_commands - 1;
    int pipes[num_pipes][2];
    pid_t pids[pipeline->num_commands];
    
    // Create all pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) == -1) {
            fprintf(stderr, "Error: pipe failed\n");
            return -1;
        }
    }
    
    // Create processes for each command in the pipeline
    for (int i = 0; i < pipeline->num_commands; i++) {
        command_t *cmd = &pipeline->commands[i];
        
        // Skip builtins in pipelines (they should be handled separately)
        if (is_builtin(cmd->args[0])) {
            fprintf(stderr, "Error: invalid command\n");
            // Clean up pipes
            for (int j = 0; j < num_pipes; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return -1;
        }
        
        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process
            
            // Set up input pipe (if not first command)
            if (i > 0) {
                if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {
                    fprintf(stderr, "dup2 failed: %s\n", strerror(errno));
                    exit(1);
                }
            }
            
            // Set up output pipe (if not last command)
            if (i < pipeline->num_commands - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    fprintf(stderr, "dup2 failed: %s\n", strerror(errno));
                    exit(1);
                }
            }
            
            // Close all pipe file descriptors
            for (int j = 0; j < num_pipes; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Set up redirection for this command
            if (setup_redirection(cmd) != 0) {
                exit(1);
            }
            
            // Execute the command
            execvp(cmd->args[0], cmd->args);
            fprintf(stderr, "Error: invalid command\n");
            exit(1);
        } else if (pids[i] < 0) {
            // Fork failed
            fprintf(stderr, "Fork error\n");
            // Clean up pipes
            for (int j = 0; j < num_pipes; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return -1;
        }
    }
    
    // Close all pipes in parent
    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all children
    int last_status = 0;
    for (int i = 0; i < pipeline->num_commands; i++) {
        int status;
        waitpid(pids[i], &status, WUNTRACED);
        
        // Only add job if the last process in pipeline was stopped
        if (i == pipeline->num_commands - 1) {
            add_job(status, pids[i], original_cmd);
            last_status = status;
        }
    }
    
    return WIFEXITED(last_status) ? WEXITSTATUS(last_status) : -1;
}