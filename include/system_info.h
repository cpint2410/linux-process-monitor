#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

typedef struct {
    unsigned long total_memory;
    unsigned long available_memory;
    double memory_usage;
} SystemInfo;

typedef struct {
    unsigned long long total_ticks;
    unsigned long long idle_ticks;
    long cpu_count;
    double cpu_usage;
} CpuInfo;

int get_system_info(SystemInfo *info);
int get_cpu_info(CpuInfo *info);

#endif