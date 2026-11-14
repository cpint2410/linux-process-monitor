#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sorting.h"

int main(void)
{
    Process processes[3] = {0};

    processes[0].pid = 300;
    processes[0].cpu_usage = 5.0;
    processes[0].resident_memory = 300;
    strcpy(processes[0].name, "python");

    processes[1].pid = 100;
    processes[1].cpu_usage = 20.0;
    processes[1].resident_memory = 100;
    strcpy(processes[1].name, "bash");

    processes[2].pid = 200;
    processes[2].cpu_usage = 10.0;
    processes[2].resident_memory = 200;
    strcpy(processes[2].name, "firefox");

    ProcessList list = {processes, 3};

    sort_processes(&list, SORT_PID);
    assert(list.items[0].pid == 100);

    sort_processes(&list, SORT_CPU);
    assert(list.items[0].cpu_usage == 20.0);

    sort_processes(&list, SORT_MEMORY);
    assert(list.items[0].resident_memory == 300);

    sort_processes(&list, SORT_NAME);
    assert(strcmp(list.items[0].name, "bash") == 0);

    printf("Sorting tests passed!\n");

    return 0;
}