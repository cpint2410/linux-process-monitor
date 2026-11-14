# Memory Monitoring

## System Memory

System memory statistics are retrieved from `/proc/meminfo`.

The application reads:

- MemTotal: Total physical memory.
- MemAvailable: Estimated available memory.

Used memory is calculated as:

Used Memory = MemTotal - MemAvailable

Memory utilisation is:

Memory Usage (%) = (Used Memory / MemTotal) × 100

## Process Memory

Process memory information is retrieved from `/proc/[PID]/statm`.

### Virtual Memory Size (VSZ)

Represents the total virtual address space allocated to a process.

### Resident Set Size (RSS)

Represents the amount of physical memory currently resident in RAM for a process.

RSS and VSZ are different measurements and should not be treated as interchangeable.

## Unit Conversion

Memory values reported in bytes are converted into megabytes for terminal display.

Linux process memory statistics may change between reads because processes continue running while monitoring takes place.