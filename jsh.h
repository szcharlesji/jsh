#ifndef JSH_H
#define JSH_H

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 1000
#define MAX_JOBS 100
#define MAX_ARGS (MAX_LEN / 2 + 1)

// Operator types
typedef enum {
  OP_NONE = 0,
  OP_PIPE = 1,
  OP_REDIRECT_OUT = 2,
  OP_REDIRECT_APPEND = 3,
  OP_REDIRECT_IN = 4
} operator_t;

// Command structure for parsed commands
typedef struct {
  char **args;
  int argc;
  operator_t redirect_type;
  char *redirect_file;
} command_t;

// Pipeline structure
typedef struct {
  command_t *commands;
  int num_commands;
} pipeline_t;

// Function declarations from other modules
extern int num_jobs;

#endif
