# CPU Scheduling Simulator (C++ / Qt 6)

A desktop Operating Systems mini-project that visualizes CPU scheduling and reports process statistics.

## Algorithms included

- **FCFS** — First Come, First Served (non-preemptive)
- **SJF** — Shortest Job First (non-preemptive)
- **SRTF** — Shortest Remaining Time First (preemptive)
- **Priority** — selectable non-preemptive or preemptive; smaller priority number means higher priority
- **Round Robin** — preemptive; configurable time quantum

All algorithms account for arrival times and CPU idle periods. If processes arrive at the same time, input order breaks ties. For preemptive algorithms, a process that is already running continues on an equal remaining-time/priority tie. Round Robin queues arrivals that occur during a time slice before re-queuing the preempted process.

## Features

- Add, edit, and remove processes in the input table
- Visual Gantt chart with time labels and idle slots
- Per-process completion, turnaround, waiting, and response time
- Average waiting, turnaround, and response times
- Input validation for duplicate IDs, negative arrival times, non-positive burst times, and invalid quantum

## Requirements

- C++17 compiler
- CMake 3.21+
- Qt 6 Widgets development package

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev
```

### Windows

Install Qt 6 (including **Qt Widgets**) and CMake. Open the project folder in Qt Creator, select a Qt 6 kit, then build and run. Alternatively, configure CMake with the Qt installation available in `CMAKE_PREFIX_PATH`.

### macOS

Install Qt 6 and CMake (for example, using the official Qt installer and Homebrew for CMake), then open the folder in Qt Creator.

## Build from terminal

Run these commands from the project root:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Run the executable:
- Linux: `./build/CPUSchedulingSimulator`
- Windows: `build\\Release\\CPUSchedulingSimulator.exe` (or the path produced by your generator)
- macOS: `./build/CPUSchedulingSimulator`

If CMake cannot find Qt, configure the Qt prefix, for example:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/gcc_64"
```

## Quick test data

Enter these rows (the table includes a sample set on first launch):

| PID | Arrival | Burst | Priority |
|---|---:|---:|---:|
| P1 | 0 | 5 | 2 |
| P2 | 1 | 3 | 1 |
| P3 | 2 | 1 | 3 |

Expected FCFS order: `P1 [0-5]`, `P2 [5-8]`, `P3 [8-9]`.
Expected FCFS statistics:
- P1: completion 5, turnaround 5, waiting 0, response 0
- P2: completion 8, turnaround 7, waiting 4, response 4
- P3: completion 9, turnaround 7, waiting 6, response 6
- Average waiting = 3.333..., average turnaround = 6.333...

For Round Robin, try quantum `2`. For Priority, lower numeric priority means higher scheduling priority.

## Viva talking points

- `Turnaround = Completion - Arrival`
- `Waiting = Turnaround - Burst`
- `Response = First Start - Arrival`
- In preemptive algorithms, a process can have multiple Gantt segments.
- An idle segment means no process had arrived or remained ready to run.
- This simulator models scheduling decisions; it does not change the host operating system's real CPU scheduler.
