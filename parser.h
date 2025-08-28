#ifndef PARSER_H
#define PARSER_H

#include "jsh.h"

// Parsing functions
pipeline_t *parse_command_line(char *command);
void free_pipeline(pipeline_t *pipeline);
void free_command(command_t *cmd);

// Built-in command checking
bool is_builtin(const char *cmd);
int execute_builtin(char **args);

// Utility functions
int validate_command_syntax(char **args, int num_args);

#endif
