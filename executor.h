#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "jsh.h"
#include "parser.h"

// Command execution functions
int execute_pipeline(pipeline_t *pipeline, char *original_cmd);
int execute_single_command(command_t *cmd);
int setup_redirection(command_t *cmd);
int check_external_command(const char *cmd);

#endif
