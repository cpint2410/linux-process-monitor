#include "sorting.h"

#include <stdlib.h>
#include <string.h>

static int compare_pid(const void *a, const void *b)
{
    const Process *x = a;
    const Process *y = b;

    return (x->pid > y->pid) - (x->pid < y->pid);
}

static int compare_name(const void *a, const void *b)
{
    const Process *x = a;
    const Process *y = b;

    return strcmp(x->name, y->name);
}

static int compare_cpu(const void *a, const void *b)
{
    const Process *x = a;
    const Process *y = b;

    return (x->cpu_usage < y->cpu_usage) -
           (x->cpu_usage > y->cpu_usage);
}

static int compare_memory(const void *a, const void *b)
{
    const Process *x = a;
    const Process *y = b;

    return (x->resident_memory < y->resident_memory) -
           (x->resident_memory > y->resident_memory);
}

void sort_processes(ProcessList *list, SortMode mode)
{
    int (*comparator)(const void *, const void *) = compare_pid;

    switch (mode) {
        case SORT_NAME:
            comparator = compare_name;
            break;
        case SORT_CPU:
            comparator = compare_cpu;
            break;
        case SORT_MEMORY:
            comparator = compare_memory;
            break;
        case SORT_PID:
        default:
            comparator = compare_pid;
            break;
    }

    qsort(list->items, list->count,
          sizeof(Process), comparator);
}