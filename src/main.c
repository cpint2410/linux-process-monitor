#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#include "process.h"
#include "system_info.h"
#include "search.h"
#include "sorting.h"

static struct termios original_terminal;

static void restore_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_terminal);
}

static int setup_terminal(void)
{
    if (tcgetattr(STDIN_FILENO, &original_terminal) == -1)
        return -1;

    struct termios raw = original_terminal;

    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        return -1;

    atexit(restore_terminal);
    return 0;
}

static void calculate_cpu(
    ProcessList *previous,
    ProcessList *current,
    unsigned long long total_delta,
    long cores)
{
    if (!total_delta)
        return;

    for (size_t i = 0; i < current->count; i++) {
        Process *now = &current->items[i];

        for (size_t j = 0; j < previous->count; j++) {
            Process *before = &previous->items[j];

            if (now->pid == before->pid &&
                now->start_time == before->start_time) {

                if (now->cpu_ticks >= before->cpu_ticks) {
                    now->cpu_usage =
                        100.0 *
                        (double)(now->cpu_ticks - before->cpu_ticks) *
                        cores / total_delta;
                }

                break;
            }
        }
    }
}

static int refresh(
    ProcessList *previous,
    CpuInfo *previous_cpu,
    SystemInfo *memory,
    double *system_cpu)
{
    ProcessList next = {0};
    CpuInfo cpu = {0};
    SystemInfo mem = {0};

    if (get_processes(&next) != 0 ||
        get_cpu_info(&cpu) != 0 ||
        get_system_info(&mem) != 0) {
        free_processes(&next);
        return -1;
    }

    unsigned long long delta =
        cpu.total_ticks - previous_cpu->total_ticks;

    unsigned long long idle_delta =
        cpu.idle_ticks - previous_cpu->idle_ticks;

    if (delta > 0 && idle_delta <= delta) {
        *system_cpu =
            100.0 * (double)(delta - idle_delta) / delta;

        calculate_cpu(previous, &next, delta, cpu.cpu_count);
    } else {
        *system_cpu = 0.0;
    }

    /* Release old snapshot and transfer ownership. */
    free_processes(previous);

    *previous = next;
    *previous_cpu = cpu;
    *memory = mem;

    return 0;
}

static void display(
    const ProcessList *processes,
    const SystemInfo *memory,
    double cpu,
    const char *query,
    SortMode mode,
    int interval)
{
    ProcessList filtered = {0};

    if (query[0]) {
        search_processes(processes, &filtered, query);
    } else {
        filtered.items = malloc(
            processes->count * sizeof(Process));

        if (filtered.items) {
            memcpy(filtered.items, processes->items,
                   processes->count * sizeof(Process));
            filtered.count = processes->count;
        }
    }

    sort_processes(&filtered, mode);

    printf("\033[2J\033[H");
    printf("========== LINUX PROCESS MONITOR ==========\n\n");

    printf("CPU Usage: %.2f%% | Memory Usage: %.2f%%\n",
           cpu, memory->memory_usage);

    printf("Processes: %zu | Refresh: %ds\n\n",
           filtered.count, interval);

    printf("%-8s %-22s %-7s %-8s %-10s %-8s\n",
           "PID", "PROCESS", "STATE", "PPID",
           "RSS(MB)", "CPU%");

    printf("---------------------------------------------------------------\n");

    for (size_t i = 0; i < filtered.count; i++) {
        Process *p = &filtered.items[i];

        printf("%-8d %-22.22s %-7c %-8d %-10.2f %-8.2f\n",
               p->pid, p->name, p->state, p->ppid,
               p->resident_memory / 1048576.0,
               p->cpu_usage);
    }

    printf("\nSearch: %s\n", query);
    printf("Sort: %d | Refresh: %d seconds\n", mode, interval);

    printf("\nControls:\n");
    printf("Q = Quit | R = Refresh | S = Search\n");
    printf("1 = PID | 2 = Name | 3 = CPU | 4 = Memory\n");
    printf("+ / - = Change refresh interval\n");

    fflush(stdout);
    free_processes(&filtered);
}

int main(void)
{
    if (setup_terminal() != 0) {
        fprintf(stderr, "Interactive terminal required.\n");
        return 1;
    }

    ProcessList previous = {0};
    CpuInfo previous_cpu = {0};
    SystemInfo memory = {0};

    double system_cpu = 0.0;
    int interval = 2;
    int running = 1;
    SortMode mode = SORT_CPU;
    char query[128] = "";

    if (get_processes(&previous) != 0 ||
        get_cpu_info(&previous_cpu) != 0) {
        fprintf(stderr, "Initial sampling failed.\n");
        free_processes(&previous);
        return 1;
    }

    struct timespec delay = {.tv_sec = 1, .tv_nsec = 0};
    nanosleep(&delay, NULL);

    if (refresh(&previous, &previous_cpu,
            &memory, &system_cpu) != 0) {
        fprintf(stderr, "Initial refresh failed.\n");
        free_processes(&previous);
        return 1;
    }

    int dirty = 1;

    while (running) {
        if (dirty) {
            display(&previous, &memory, system_cpu,
                    query, mode, interval);
            dirty = 0;
        }

        fd_set set;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        struct timeval timeout = {
            .tv_sec = interval,
            .tv_usec = 0
        };

        int ready = select(STDIN_FILENO + 1, &set,
                           NULL, NULL, &timeout);

        if (ready > 0 && FD_ISSET(STDIN_FILENO, &set)) {
            char key;

            if (read(STDIN_FILENO, &key, 1) != 1)
                continue;

            switch (key) {
                case 'q':
                case 'Q':
                    running = 0;
                    break;

                case 'r':
                case 'R':
                    dirty = 1;
                    break;

                case '1':
                    mode = SORT_PID;
                    dirty = 1;
                    break;

                case '2':
                    mode = SORT_NAME;
                    dirty = 1;
                    break;

                case '3':
                    mode = SORT_CPU;
                    dirty = 1;
                    break;

                case '4':
                    mode = SORT_MEMORY;
                    dirty = 1;
                    break;

                case '+':
                    if (interval < 10)
                        interval++;
                    dirty = 1;
                    break;

                case '-':
                    if (interval > 1)
                        interval--;
                    dirty = 1;
                    break;

                case 's':
                case 'S': {
                    restore_terminal();

                    printf("\nEnter process name or PID: ");
                    fflush(stdout);

                    if (fgets(query, sizeof(query), stdin)) {
                        query[strcspn(query, "\n")] = '\0';
                    }

                    if (setup_terminal() != 0)
                        running = 0;

                    dirty = 1;
                    break;
                }
            }
        } else if (ready == 0) {
            dirty = 1;
        }

        if (running && dirty) {
            if (refresh(&previous, &previous_cpu,
                        &memory, &system_cpu) != 0) {
                running = 0;
            }
        }
    }

    free_processes(&previous);

    printf("\nLinux Process Monitor closed.\n");

    return 0;
}