#ifndef SEARCH_H
#define SEARCH_H

#include "process.h"

void search_processes(
    const ProcessList *source,
    ProcessList *result,
    const char *query
);

#endif