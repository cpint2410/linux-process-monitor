#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "search.h"

int main(void)
{
    Process processes[3] = {0};

    processes[0].pid = 100;
    strcpy(processes[0].name, "bash");

    processes[1].pid = 200;
    strcpy(processes[1].name, "firefox");

    processes[2].pid = 300;
    strcpy(processes[2].name, "python");

    ProcessList source = {processes, 3};
    ProcessList result = {0};

    search_processes(&source, &result, "bash");

    assert(result.count == 1);
    assert(strcmp(result.items[0].name, "bash") == 0);

    free_processes(&result);

    search_processes(&source, &result, "200");

    assert(result.count == 1);
    assert(result.items[0].pid == 200);

    free_processes(&result);

    search_processes(&source, &result, "nonexistent");

    assert(result.count == 0);

    free_processes(&result);

    printf("Search tests passed!\n");

    return 0;
}