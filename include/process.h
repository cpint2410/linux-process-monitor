#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include <stddef.h>

#define PROCESS_NAME_LEN 256

typedef struct {
    pid_t pid;
    pid_t ppid;
    uid_t uid;

    char name[PROCESS_NAME_LEN];
    char state;

    unsigned long virtual_memory;
    long resident_memory;

    unsigned long long cpu_ticks;
    unsigned long long start_time;
    double cpu_usage;

} Process;

typedef struct {
    Process *items;
    size_t count;
} ProcessList;

int get_processes(ProcessList *list);
void free_processes(ProcessList *list);

#endif