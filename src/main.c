#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "process.h"
#include "system_info.h"

static void calculate_cpu_usage(
    ProcessList *previous,
    ProcessList *current,
    unsigned long long total_delta,
    long cpu_count)
{
    if (total_delta == 0)
        return;

    for (size_t i = 0; i < current->count; i++) {

        Process *now = &current->items[i];

        now->cpu_usage = 0.0;

        for (size_t j = 0; j < previous->count; j++) {

            Process *before = &previous->items[j];

            if (now->pid == before->pid &&
                now->start_time == before->start_time) {

                if (now->cpu_ticks >= before->cpu_ticks) {

                    unsigned long long difference =
                        now->cpu_ticks - before->cpu_ticks;

                    now->cpu_usage =
                        100.0 * (double)difference *
                        (double)cpu_count /
                        (double)total_delta;
                }

                break;
            }
        }
    }
}

int main(void)
{
    ProcessList previous = {0};
    ProcessList current = {0};

    CpuInfo cpu_before = {0};
    CpuInfo cpu_after = {0};

    SystemInfo memory = {0};

    printf("Collecting CPU statistics...\n");

    if (get_processes(&previous) != 0 ||
        get_cpu_info(&cpu_before) != 0) {

        fprintf(stderr, "Initial system sampling failed.\n");
        free_processes(&previous);
        return 1;
    }

    struct timespec delay = {
        .tv_sec = 1,
        .tv_nsec = 0
    };

    while (nanosleep(&delay, &delay) == -1) {
        continue;
    }

    if (get_processes(&current) != 0 ||
        get_cpu_info(&cpu_after) != 0 ||
        get_system_info(&memory) != 0) {

        fprintf(stderr, "Second system sampling failed.\n");

        free_processes(&previous);
        free_processes(&current);

        return 1;
    }

    unsigned long long total_delta =
        cpu_after.total_ticks - cpu_before.total_ticks;

    unsigned long long idle_delta =
        cpu_after.idle_ticks - cpu_before.idle_ticks;

    if (total_delta == 0 || idle_delta > total_delta) {
        fprintf(stderr, "Invalid CPU statistics.\n");

        free_processes(&previous);
        free_processes(&current);

        return 1;
    }

    cpu_after.cpu_usage =
        100.0 * (double)(total_delta - idle_delta)
        / (double)total_delta;

    calculate_cpu_usage(
        &previous,
        &current,
        total_delta,
        cpu_after.cpu_count
    );

    printf("\033[2J\033[H");

    printf("============================================================\n");
    printf("                 LINUX PROCESS MONITOR\n");
    printf("============================================================\n\n");

    printf("SYSTEM INFORMATION\n");
    printf("------------------------------------------------------------\n");

    printf("CPU Usage:        %.2f%%\n", cpu_after.cpu_usage);
    printf("CPU Cores:        %ld\n", cpu_after.cpu_count);

    printf("Total Memory:     %.2f GB\n",
           memory.total_memory / 1024.0 / 1024.0);

    printf("Available Memory: %.2f GB\n",
           memory.available_memory / 1024.0 / 1024.0);

    printf("Memory Usage:     %.2f%%\n", memory.memory_usage);

    printf("Total Processes:  %zu\n\n", current.count);

    printf("%-8s %-22s %-7s %-8s %-10s %-10s %-8s\n",
           "PID", "PROCESS", "STATE", "PPID",
           "RSS(MB)", "VSZ(MB)", "CPU%");

    printf("--------------------------------------------------------------------------\n");

    for (size_t i = 0; i < current.count; i++) {

        Process *p = &current.items[i];

        printf("%-8d %-22.22s %-7c %-8d %-10.2f %-10.2f %-8.2f\n",
               p->pid,
               p->name,
               p->state,
               p->ppid,
               p->resident_memory / 1024.0 / 1024.0,
               p->virtual_memory / 1024.0 / 1024.0,
               p->cpu_usage);
    }

    printf("\nCPU sampling interval: 1 second\n");
    printf("Total processes displayed: %zu\n", current.count);

    free_processes(&previous);
    free_processes(&current);

    return 0;
}