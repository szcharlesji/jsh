#include "jobs.h"

job_t jobs[MAX_JOBS];
int num_jobs = 0;

void signal_handler(int sig) {
    // Empty handler for SIGINT, SIGQUIT, SIGTSTP
    // These signals are ignored in the parent shell
    (void)sig; // Suppress unused parameter warning
}

void setup_signal_handlers(void) {
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTSTP, signal_handler);
}

int add_job(int status, pid_t pid, char *cmdStr) {
    if (WIFSTOPPED(status)) {
        if (num_jobs < MAX_JOBS) {
            jobs[num_jobs].cmd = malloc(strlen(cmdStr) + 1);
            if (jobs[num_jobs].cmd == NULL) {
                fprintf(stderr, "Error: memory allocation failed\n");
                return -1;
            }
            strcpy(jobs[num_jobs].cmd, cmdStr);
            jobs[num_jobs].pid = pid;
            jobs[num_jobs].status = JOB_STOPPED;
            num_jobs++;
            return 0;
        } else {
            fprintf(stderr, "Error: too many jobs\n");
            return -1;
        }
    }
    return 0;
}

void remove_job(int job_index) {
    if (job_index < 0 || job_index >= num_jobs) {
        return;
    }
    
    if (jobs[job_index].cmd) {
        free(jobs[job_index].cmd);
    }
    
    // Shift remaining jobs down
    for (int i = job_index; i < num_jobs - 1; i++) {
        jobs[i] = jobs[i + 1];
    }
    num_jobs--;
}

void list_jobs(void) {
    for (int i = 0; i < num_jobs; i++) {
        printf("[%d] %s\n", i + 1, jobs[i].cmd);
    }
}

int fg_job(int job_num) {
    if (job_num < 1 || job_num > num_jobs) {
        fprintf(stderr, "Error: invalid job\n");
        return -1;
    }
    
    int job_index = job_num - 1;
    pid_t pid = jobs[job_index].pid;
    char *cmdName = malloc(strlen(jobs[job_index].cmd) + 1);
    if (cmdName == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return -1;
    }
    strcpy(cmdName, jobs[job_index].cmd);
    
    // Remove job from array before continuing
    remove_job(job_index);
    
    // Continue the job and wait for it
    kill(pid, SIGCONT);
    int status;
    waitpid(pid, &status, WUNTRACED);
    
    // If job was stopped again, add it back
    add_job(status, pid, cmdName);
    
    free(cmdName);
    return 0;
}

void cleanup_jobs(void) {
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].cmd) {
            free(jobs[i].cmd);
        }
    }
    num_jobs = 0;
}