#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

typedef struct {
    unsigned long total_memory;
    unsigned long available_memory;
    double memory_usage;
} SystemInfo;

int get_system_info(SystemInfo *info);

#endif