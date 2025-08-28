#include "executor.h"
#include "jobs.h"
#include "jsh.h"
#include "parser.h"

int main(void) {
  char command[MAX_LEN];
  bool flag = true;
  char wd[MAX_LEN];
  char *base;

  setup_signal_handlers();

  while (flag) {
    // Display prompt
    getcwd(wd, sizeof(wd));
    base = basename(wd);
    printf("%s $ ", base);
    fflush(stdout);

    // Read command
    if (fgets(command, MAX_LEN, stdin) == NULL) {
      printf("\n");
      flag = false;
      break;
    }

    // Remove newline
    size_t len = strlen(command);
    if (len > 0 && command[len - 1] == '\n') {
      command[len - 1] = '\0';
    }

    // Skip empty commands
    if (strlen(command) == 0) {
      continue;
    }

    // Save original command for job control
    char *original_cmd = malloc(strlen(command) + 1);
    if (original_cmd) {
      strcpy(original_cmd, command);
    }

    // Parse the command line
    pipeline_t *pipeline = parse_command_line(command);
    if (!pipeline) {
      // Empty command or parsing error
      if (original_cmd)
        free(original_cmd);
      continue;
    }

    // Check for syntax errors
    if (pipeline->num_commands == 0) {
      fprintf(stderr, "Error: invalid command\n");
      free_pipeline(pipeline);
      if (original_cmd)
        free(original_cmd);
      continue;
    }

    // Handle builtin commands for single commands
    if (pipeline->num_commands == 1 &&
        is_builtin(pipeline->commands[0].args[0])) {
      int result = execute_builtin(pipeline->commands[0].args);
      if (strcmp(pipeline->commands[0].args[0], "exit") == 0 && result == 0) {
        flag = false;
      }
    } else {
      // Execute pipeline
      execute_pipeline(pipeline, original_cmd ? original_cmd : "");
    }

    free_pipeline(pipeline);
    if (original_cmd)
      free(original_cmd);
  }

  cleanup_jobs();
  return 0;
}
