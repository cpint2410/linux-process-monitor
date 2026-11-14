# CPU Utilisation Calculations

## System CPU Usage

Linux exposes cumulative CPU time counters through `/proc/stat`.

The application takes two samples and calculates the difference between them.

The formula is:

CPU Usage (%) = ((Total Delta - Idle Delta) / Total Delta) × 100

## Process CPU Usage

Individual process CPU time is retrieved from `/proc/[PID]/stat`.

The application compares process CPU ticks between two samples.

Process CPU (%) = (Process Tick Delta / System Tick Delta) × Number of CPU Cores × 100

This calculation allows a process using one full CPU core to display approximately 100% utilisation.

## Sampling

CPU utilisation cannot be accurately calculated from a single sample.

The application therefore:

1. Collects an initial sample.
2. Waits for the configured interval.
3. Collects another sample.
4. Calculates the difference.
5. Displays the resulting percentage.

## Process Identification

Process start time is also compared to avoid incorrectly matching a newly created process with an earlier process that used the same PID.