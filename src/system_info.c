#define _POSIX_C_SOURCE 200809L

#include "system_info.h"

#include <stdio.h>
#include <unistd.h>

int get_system_info(SystemInfo *info)
{
    FILE *file = fopen("/proc/meminfo", "r");

    if (!file)
        return -1;

    char line[256];

    info->total_memory = 0;
    info->available_memory = 0;

    while (fgets(line, sizeof(line), file)) {

        unsigned long value;

        if (sscanf(line, "MemTotal: %lu", &value) == 1)
            info->total_memory = value;

        if (sscanf(line, "MemAvailable: %lu", &value) == 1)
            info->available_memory = value;
    }

    fclose(file);

    if (info->total_memory == 0 ||
        info->available_memory > info->total_memory)
        return -1;

    info->memory_usage =
        100.0 * (info->total_memory - info->available_memory)
        / info->total_memory;

    return 0;
}

int get_cpu_info(CpuInfo *info)
{
    FILE *file = fopen("/proc/stat", "r");

    if (!file)
        return -1;

    unsigned long long user, nice, system, idle;
    unsigned long long iowait, irq, softirq, steal;

    int result = fscanf(
        file,
        "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
        &user, &nice, &system, &idle,
        &iowait, &irq, &softirq, &steal
    );

    fclose(file);

    if (result != 8)
        return -1;

    info->total_ticks =
        user + nice + system + idle +
        iowait + irq + softirq + steal;

    info->idle_ticks = idle + iowait;

    info->cpu_count = sysconf(_SC_NPROCESSORS_ONLN);

    if (info->cpu_count <= 0)
        return -1;

    info->cpu_usage = 0.0;

    return 0;
}