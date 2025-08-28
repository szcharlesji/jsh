#ifndef JOBS_H
#define JOBS_H

#include "jsh.h"

// Job status
typedef enum {
  JOB_RUNNING = 0,
  JOB_STOPPED = 1,
  JOB_TERMINATED = 2
} job_status_t;

// Job struct
typedef struct {
  pid_t pid;
  char *cmd;
  job_status_t status;
} job_t;

extern job_t jobs[MAX_JOBS];
extern int num_jobs;

// Job control functions
int add_job(int status, pid_t pid, char *cmdStr);
void remove_job(int job_index);
void list_jobs(void);
int fg_job(int job_num);
void cleanup_jobs(void);

// Signal handlers
void setup_signal_handlers(void);
void signal_handler(int sig);

#endif
