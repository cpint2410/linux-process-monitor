#define _POSIX_C_SOURCE 200809L

#include "process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

static int read_process(pid_t pid, Process *p)
{
    char path[64];
    char buffer[4096];

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *file = fopen(path, "r");

    if (!file)
        return -1;

    if (!fgets(buffer, sizeof(buffer), file)) {
        fclose(file);
        return -1;
    }

    fclose(file);

    char *start = strchr(buffer, '(');
    char *end = strrchr(buffer, ')');

    if (!start || !end || end <= start)
        return -1;

    size_t name_len = (size_t)(end - start - 1);

    if (name_len >= sizeof(p->name))
        name_len = sizeof(p->name) - 1;

    memcpy(p->name, start + 1, name_len);
    p->name[name_len] = '\0';

    char *saveptr;
    char *token = strtok_r(end + 2, " ", &saveptr);

    if (!token)
        return -1;

    p->state = token[0];

    for (int field = 4; field <= 22; field++) {

        token = strtok_r(NULL, " ", &saveptr);

        if (!token)
            return -1;

        char *parse_end;

        switch (field) {

            case 4:
                p->ppid = (pid_t)strtol(token, &parse_end, 10);
                if (*parse_end != '\0')
                    return -1;
                break;

            case 14:
            case 15: {
                unsigned long long ticks =
                    strtoull(token, &parse_end, 10);

                if (*parse_end != '\0')
                    return -1;

                if (field == 14)
                    p->cpu_ticks = ticks;
                else
                    p->cpu_ticks += ticks;

                break;
            }

            case 22:
                p->start_time =
                    strtoull(token, &parse_end, 10);

                if (*parse_end != '\0')
                    return -1;
                break;

            default:
                break;
        }
    }

    snprintf(path, sizeof(path), "/proc/%d/statm", pid);

    file = fopen(path, "r");

    if (!file)
        return -1;

    unsigned long pages;
    unsigned long resident;

    int result = fscanf(file, "%lu %lu", &pages, &resident);

    fclose(file);

    if (result != 2)
        return -1;

    long page_size = sysconf(_SC_PAGESIZE);

    if (page_size <= 0)
        return -1;

    p->virtual_memory = pages * (unsigned long)page_size;

    p->resident_memory =
        (long)(resident * (unsigned long)page_size);

    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    file = fopen(path, "r");

    p->uid = (uid_t)-1;

    if (file) {
        while (fgets(buffer, sizeof(buffer), file)) {

            unsigned int uid;

            if (sscanf(buffer, "Uid:\t%u", &uid) == 1) {
                p->uid = (uid_t)uid;
                break;
            }
        }

        fclose(file);
    }

    p->pid = pid;
    p->cpu_usage = 0.0;

    return 0;
}

int get_processes(ProcessList *list)
{
    list->items = NULL;
    list->count = 0;

    DIR *dir = opendir("/proc");

    if (!dir)
        return -1;

    size_t capacity = 128;

    list->items = malloc(capacity * sizeof(Process));

    if (!list->items) {
        closedir(dir);
        return -1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        char *end;
        errno = 0;

        long value = strtol(entry->d_name, &end, 10);

        if (errno || *end != '\0' ||
            value <= 0 || value > INT32_MAX)
            continue;

        Process p = {0};

        if (read_process((pid_t)value, &p) != 0)
            continue;

        if (list->count == capacity) {

            if (capacity > SIZE_MAX / 2 / sizeof(Process)) {
                free_processes(list);
                closedir(dir);
                return -1;
            }

            capacity *= 2;

            Process *new_items = realloc(
                list->items,
                capacity * sizeof(Process)
            );

            if (!new_items) {
                free_processes(list);
                closedir(dir);
                return -1;
            }

            list->items = new_items;
        }

        list->items[list->count++] = p;
    }

    closedir(dir);

    return 0;
}

void free_processes(ProcessList *list)
{
    free(list->items);

    list->items = NULL;
    list->count = 0;
}