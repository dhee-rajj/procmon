# Process Monitor

## Overview
This is a simple Linux process monitoring tool written in C. It tracks running processes and detects when new processes are created or existing processes are terminated.

## Features
- Lists active processes using the `/proc` filesystem.
- Monitors changes in process creation and termination.
- Logs events for new and terminated processes in `process_log.txt`.
- Differentiates between **system** and **user** processes based on UID.

## How It Works
1. Reads process information from `/proc/[pid]/status`.
2. Compares the current process list with the previous one to detect changes.
3. Logs and prints process creation and termination events.
4. Runs in a loop, updating every `SLEEP_TIME` seconds.

## Installation
Compile the program using GCC:
```sh
gcc process_monitor.c -o process_monitor
```

## Usage
Run the executable with:
```sh
./process_monitor
```
Press `Ctrl+C` to stop monitoring.

## Log Format
The tool logs process events in `process_log.txt` in the format:
```
[NEW] [USER] PID: 1234 (bash) | PPID: 567 | UID: 1000
[TERMINATED] [SYSTEM] PID: 4321 (cron) | PPID: 1 | UID: 0
```
- **NEW**: A new process started.
- **TERMINATED**: A process has exited.
- **SYSTEM**: UID < 1000 (system processes).
- **USER**: UID >= 1000 (user processes).

## Notes
- Requires **Linux** since it depends on `/proc`.
- Running as **root** may be required for full process visibility.

## License
MIT License.

