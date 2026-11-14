#include <stdio.h>
#include <stdlib.h>

#include "process.h"
#include "system_info.h"

int main(void)
{
    ProcessList processes = {0};
    SystemInfo system;

    printf("\033[2J\033[H");

    printf("============================================\n");
    printf("          LINUX PROCESS MONITOR\n");
    printf("============================================\n\n");

    if (get_system_info(&system) != 0) {
        fprintf(stderr, "Error reading system memory information.\n");
        return 1;
    }

    if (get_processes(&processes) != 0) {
        fprintf(stderr, "Error retrieving process information.\n");
        return 1;
    }

    printf("SYSTEM INFORMATION\n");
    printf("--------------------------------------------\n");

    printf("Total Memory:     %.2f GB\n",
           system.total_memory / 1024.0 / 1024.0);

    printf("Available Memory: %.2f GB\n",
           system.available_memory / 1024.0 / 1024.0);

    printf("Memory Usage:     %.2f%%\n",
           system.memory_usage);

    printf("Total Processes:  %zu\n\n", processes.count);

    printf("%-8s %-24s %-8s %-10s %-10s\n",
           "PID", "PROCESS", "STATE", "RSS (MB)", "VSZ (MB)");

    printf("------------------------------------------------------------\n");

    for (size_t i = 0; i < processes.count; i++) {
        Process *p = &processes.items[i];

        printf("%-8d %-24.24s %-8c %-10.2f %-10.2f\n",
               p->pid,
               p->name,
               p->state,
               p->resident_memory / 1024.0 / 1024.0,
               p->virtual_memory / 1024.0 / 1024.0);
    }

    printf("\nTotal processes displayed: %zu\n",
           processes.count);

    free_processes(&processes);

    return 0;
}