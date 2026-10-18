# Test Cases

## Test 1 — FCFS baseline

Input:
| PID | Arrival | Burst | Priority |
|---|---:|---:|---:|
| P1 | 0 | 5 | 2 |
| P2 | 1 | 3 | 1 |
| P3 | 2 | 1 | 3 |

Expected order: P1 (0–5), P2 (5–8), P3 (8–9)

Expected:
- P1: CT=5, TAT=5, WT=0, RT=0
- P2: CT=8, TAT=7, WT=4, RT=4
- P3: CT=9, TAT=7, WT=6, RT=6
- Average WT=3.33, average TAT=6.33, average RT=3.33

## Test 2 — CPU idle time

| PID | Arrival | Burst | Priority |
|---|---:|---:|---:|
| P1 | 3 | 2 | 1 |
| P2 | 7 | 1 | 2 |

Expected timeline contains IDLE (0–3), P1 (3–5), IDLE (5–7), P2 (7–8).

## Test 3 — SRTF preemption

| PID | Arrival | Burst | Priority |
|---|---:|---:|---:|
| P1 | 0 | 8 | 2 |
| P2 | 1 | 4 | 1 |
| P3 | 2 | 2 | 3 |

Expected SRTF: P1 (0–1), P2 (1–2), P3 (2–4), P2 (4–7), P1 (7–14).  
This verifies that shorter jobs can preempt the running process.

## Test 4 — Round Robin

Use Test 1 with quantum 2. Verify that each ready process gets at most 2 time units per turn and that arrivals during a time slice enter the queue before the running process is re-queued.

## Test 5 — Priority preemption

| PID | Arrival | Burst | Priority |
|---|---:|---:|---:|
| P1 | 0 | 6 | 3 |
| P2 | 2 | 2 | 1 |

With preemptive Priority (smaller number = higher priority), P1 runs 0–2, P2 runs 2–4, then P1 runs 4–8.

## Validation tests

- Duplicate PIDs: simulation must display a validation error.
- Burst time `0`: rejected.
- Negative arrival: rejected.
- Empty process table: rejected.
- Round Robin quantum `0`: prevented by the GUI spin box.
