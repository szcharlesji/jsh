#include "parser.h"
#include "jobs.h"

bool is_builtin(const char *cmd) {
  return (strcmp(cmd, "exit") == 0 || strcmp(cmd, "cd") == 0 ||
          strcmp(cmd, "jobs") == 0 || strcmp(cmd, "fg") == 0);
}

int execute_builtin(char **args) {
  if (strcmp(args[0], "exit") == 0) {
    if (args[1] != NULL) {
      fprintf(stderr, "Error: invalid command\n");
      return 1;
    }
    if (num_jobs > 0) {
      fprintf(stderr, "Error: there are suspended jobs\n");
      return 1;
    }
    cleanup_jobs();
    exit(0);
  }

  if (strcmp(args[0], "cd") == 0) {
    if (args[1] == NULL) {
      fprintf(stderr, "Error: invalid command\n");
      return 1;
    }
    if (chdir(args[1]) != 0) {
      fprintf(stderr, "Error: invalid directory\n");
      return 1;
    }
    return 0;
  }

  if (strcmp(args[0], "jobs") == 0) {
    if (args[1] != NULL) {
      fprintf(stderr, "Error: invalid command\n");
      return 1;
    }
    list_jobs();
    return 0;
  }

  if (strcmp(args[0], "fg") == 0) {
    if (args[1] == NULL || args[2] != NULL) {
      fprintf(stderr, "Error: invalid command\n");
      return 1;
    }
    int job_num = atoi(args[1]);
    return fg_job(job_num);
  }

  return -1; // Not a builtin
}

int validate_command_syntax(char **args, int num_args) {
  for (int i = 0; i < num_args; i++) {
    if (args[i] == NULL)
      break;

    // Check for operators at boundaries
    if (strcmp(args[i], "|") == 0 || strcmp(args[i], ">") == 0 ||
        strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0) {

      if (i == 0 || i == num_args - 1 || args[i + 1] == NULL) {
        return -1;
      }
    }
  }
  return 0;
}

operator_t get_operator_type(const char *str) {
  if (strcmp(str, "|") == 0)
    return OP_PIPE;
  if (strcmp(str, ">") == 0)
    return OP_REDIRECT_OUT;
  if (strcmp(str, ">>") == 0)
    return OP_REDIRECT_APPEND;
  if (strcmp(str, "<") == 0)
    return OP_REDIRECT_IN;
  return OP_NONE;
}

pipeline_t *parse_command_line(char *command) {
  pipeline_t *pipeline = malloc(sizeof(pipeline_t));
  if (!pipeline)
    return NULL;

  // Tokenize the command
  char *args[MAX_ARGS];
  int num_args = 0;

  args[0] = strtok(command, " \n");
  while (args[num_args] != NULL && num_args < MAX_ARGS - 1) {
    num_args++;
    args[num_args] = strtok(NULL, " \n");
  }

  if (num_args == 0) {
    free(pipeline);
    return NULL;
  }

  // Validate syntax
  if (validate_command_syntax(args, num_args) != 0) {
    free(pipeline);
    return NULL;
  }

  // Count pipes to determine number of commands
  int pipe_count = 0;
  for (int i = 0; i < num_args; i++) {
    if (args[i] && strcmp(args[i], "|") == 0) {
      pipe_count++;
    }
  }

  pipeline->num_commands = pipe_count + 1;
  pipeline->commands = malloc(pipeline->num_commands * sizeof(command_t));
  if (!pipeline->commands) {
    free(pipeline);
    return NULL;
  }

  // Parse commands separated by pipes
  int cmd_idx = 0;
  int arg_start = 0;

  for (int i = 0; i <= num_args; i++) {
    if (i == num_args || (args[i] && strcmp(args[i], "|") == 0)) {
      // Process command from arg_start to i-1
      command_t *cmd = &pipeline->commands[cmd_idx];
      cmd->redirect_type = OP_NONE;
      cmd->redirect_file = NULL;

      // Count arguments for this command
      int cmd_argc = 0;
      for (int j = arg_start; j < i; j++) {
        operator_t op = get_operator_type(args[j]);
        if (op == OP_REDIRECT_OUT || op == OP_REDIRECT_APPEND ||
            op == OP_REDIRECT_IN) {
          cmd->redirect_type = op;
          if (j + 1 < i) {
            cmd->redirect_file = strdup(args[j + 1]);
            j++; // Skip the filename
          }
          break;
        } else if (op == OP_NONE) {
          cmd_argc++;
        }
      }

      // Allocate and copy arguments
      cmd->args = malloc((cmd_argc + 1) * sizeof(char *));
      cmd->argc = cmd_argc;
      int arg_idx = 0;

      for (int j = arg_start; j < i && arg_idx < cmd_argc; j++) {
        operator_t op = get_operator_type(args[j]);
        if (op == OP_REDIRECT_OUT || op == OP_REDIRECT_APPEND ||
            op == OP_REDIRECT_IN) {
          break;
        } else {
          cmd->args[arg_idx] = strdup(args[j]);
          arg_idx++;
        }
      }
      cmd->args[cmd_argc] = NULL;

      cmd_idx++;
      arg_start = i + 1;
    }
  }

  return pipeline;
}

void free_command(command_t *cmd) {
  if (!cmd)
    return;

  if (cmd->args) {
    for (int i = 0; i < cmd->argc; i++) {
      free(cmd->args[i]);
    }
    free(cmd->args);
  }

  if (cmd->redirect_file) {
    free(cmd->redirect_file);
  }
}

void free_pipeline(pipeline_t *pipeline) {
  if (!pipeline)
    return;

  if (pipeline->commands) {
    for (int i = 0; i < pipeline->num_commands; i++) {
      free_command(&pipeline->commands[i]);
    }
    free(pipeline->commands);
  }

  free(pipeline);
}
