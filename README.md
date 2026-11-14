# Linux Process Monitor

A Linux-based process monitoring utility developed in C using the Linux `/proc` filesystem.

The application retrieves and analyses live system and process information, displaying CPU utilisation, memory consumption and process statistics directly in the terminal.

## Features

- Discover running Linux processes.
- Display PID, process name, state and parent PID.
- Monitor individual process CPU utilisation.
- Monitor overall system CPU utilisation.
- Display system memory usage.
- Display process Resident Set Size (RSS) and Virtual Memory Size (VSZ).
- Search processes by name or PID.
- Sort processes by CPU, memory, PID and name.
- Automatically refresh system statistics.
- Configure the refresh interval.
- Interactive terminal keyboard controls.

## Technologies

| Technology | Purpose |
| --- | --- |
| C | Application development |
| Ubuntu Linux | Development environment |
| GCC | Compilation |
| Linux `/proc` | System and process information |
| POSIX | Terminal and system interfaces |
| Make | Build automation |
| Git and GitHub | Version control |
| GDB | Debugging |
| Valgrind | Memory analysis |

## Project Structure

text

```text
linux-process-monitor/
├── src/
│   ├── main.c
│   ├── process.c
│   ├── system_info.c
│   ├── search.c
│   └── sorting.c
├── include/
│   ├── process.h
│   ├── system_info.h
│   ├── search.h
│   └── sorting.h
├── tests/
│   ├── test_search.c
│   └── test_sorting.c
├── docs/
│   ├── architecture.md
│   ├── cpu-calculations.md
│   └── memory-monitoring.md
├── Makefile
├── README.md
└── .gitignore
```

## Requirements

- Ubuntu Linux or another compatible Linux distribution.
- GCC compiler.
- GNU Make.
- Git.

Optional development tools:

- GDB
- Valgrind

## Installation

Clone the repository:

bash

```bash
git clone https://github.com/cpint2410/linux-process-monitor.git
cd linux-process-monitor
```

Install the development tools:

bash

```bash
sudo apt update
sudo apt install build-essential git gdb valgrind
```

## Compilation

Build the application:

bash

```bash
make clean
make
```

The executable is generated as:

bash

```bash
./process-monitor
```

## Usage

Start the application:

bash

```bash
./process-monitor
```

### Keyboard Controls

| Key | Function |
| --- | --- |
| Q | Quit application |
| R | Refresh display |
| S | Search processes |
| 1 | Sort by PID |
| 2 | Sort by process name |
| 3 | Sort by CPU utilisation |
| 4 | Sort by memory usage |
| + | Increase refresh interval |
| - | Decrease refresh interval |

Search accepts a process name or PID. Enter an empty search to clear the filter.

## CPU Monitoring

CPU statistics are retrieved from:

- `/proc/stat`
- `/proc/[PID]/stat`

The application compares consecutive samples to calculate CPU utilisation over time.

System CPU utilisation:

text

```text
CPU Usage (%) =
((Total Delta - Idle Delta) / Total Delta) * 100
```

Process CPU utilisation is calculated using the change in process CPU ticks relative to the change in total system CPU ticks.

See CPU Calculations.

## Memory Monitoring

System memory information is retrieved from `/proc/meminfo`.

Process memory information is retrieved from `/proc/[PID]/statm`.

The application monitors:

- Total system memory.
- Available system memory.
- Memory utilisation percentage.
- Process RSS.
- Process VSZ.

See Memory Monitoring.

## Testing

Run the automated unit tests:

bash

```bash
make test
```

The test suite covers:

- Process searching by name.
- Process searching by PID.
- Empty search results.
- Sorting by PID.
- Sorting by process name.
- Sorting by CPU utilisation.
- Sorting by memory consumption.

## Memory Analysis

Run the application using Valgrind:

bash

```bash
valgrind --leak-check=full --track-origins=yes ./process-monitor
```

The application has been tested with Valgrind, with zero reported memory errors and no outstanding heap allocations in the tested run.

## Documentation

Additional technical documentation:

- System Architecture
- CPU Calculations
- Memory Monitoring

## Engineering Challenges

### Process identification

Linux processes can terminate while their information is being read. The application handles unavailable process entries.

### CPU utilisation

CPU percentages require multiple samples rather than simply displaying raw CPU time.

### Memory management

Dynamic memory allocation is used to manage process information. Allocated resources must be released correctly.

### Permissions

Some process information may be inaccessible to ordinary users. Monitoring does not require root privileges for normal operation.

### Performance

The application uses periodic refreshes to monitor system activity without continuously polling the filesystem.

## Future Improvements

- Interactive ncurses interface.
- Process filtering by user.
- Process tree visualisation.
- Historical resource statistics.
- CSV export.
- Disk and network monitoring.

## Author

Developed as a Linux systems programming and software engineering portfolio project.

## License

See the repository's LICENSE file.