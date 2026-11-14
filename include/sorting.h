#ifndef SORTING_H
#define SORTING_H

#include "process.h"

typedef enum {
    SORT_PID,
    SORT_NAME,
    SORT_CPU,
    SORT_MEMORY
} SortMode;

void sort_processes(ProcessList *list, SortMode mode);

#endif