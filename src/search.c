#define _GNU_SOURCE
#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void search_processes(
    const ProcessList *source,
    ProcessList *result,
    const char *query)
{
    result->items = NULL;
    result->count = 0;

    result->items = malloc(source->count * sizeof(Process));

    if (!result->items && source->count > 0)
        return;

    for (size_t i = 0; i < source->count; i++) {
        const Process *p = &source->items[i];

        char pid[32];
        snprintf(pid, sizeof(pid), "%d", p->pid);

        if (strcasestr(p->name, query) ||
            strstr(pid, query)) {
            result->items[result->count++] = *p;
        }
    }
}