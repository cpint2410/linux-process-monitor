# System Architecture

## Overview

Linux Process Monitor is a modular Linux systems programming application written in C.

It retrieves system and process information directly from the Linux `/proc` filesystem.

## Architecture

The application is divided into five main modules.

### 1\. Main Application

- Initialises the application.
- Coordinates monitoring modules.
- Manages the refresh loop.
- Handles keyboard input and termination.

### 2\. Process Monitoring

- Discovers running processes.
- Reads process statistics from `/proc/[PID]/stat`.
- Retrieves process memory information.
- Stores process information in dynamically allocated structures.

### 3\. System Information

- Reads CPU statistics from `/proc/stat`.
- Reads memory statistics from `/proc/meminfo`.
- Calculates system resource utilisation.

### 4\. Search and Sorting

- Searches processes by name or PID.
- Sorts processes by CPU, memory, PID and name.

### 5\. Display

- Renders system statistics.
- Displays process information.
- Provides interactive terminal controls.

## Design Principles

- Modular source code.
- Separation of responsibilities.
- Dynamic memory management.
- Error handling for inaccessible processes.
- Read-only interaction with system information.
- No database or external monitoring service required.