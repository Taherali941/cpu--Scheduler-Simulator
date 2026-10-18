#pragma once

#include <QString>
#include <QVector>
#include <optional>

struct ProcessInput {
    QString pid;
    int arrival = 0;
    int burst = 1;
    int priority = 1;
};

struct GanttSegment {
    QString pid; // "IDLE" for idle time
    int start = 0;
    int end = 0;
};

struct ProcessStats {
    QString pid;
    int arrival = 0;
    int burst = 0;
    int priority = 0;
    int completion = 0;
    int turnaround = 0;
    int waiting = 0;
    int response = 0;
};

struct ScheduleResult {
    QVector<GanttSegment> timeline;
    QVector<ProcessStats> stats; // in original input order
    double averageWaiting = 0.0;
    double averageTurnaround = 0.0;
    double averageResponse = 0.0;
};

enum class Algorithm {
    FCFS,
    SJF,
    SRTF,
    PriorityNonPreemptive,
    PriorityPreemptive,
    RoundRobin
};

class Scheduler {
public:
    static std::optional<ScheduleResult> run(
        const QVector<ProcessInput>& processes,
        Algorithm algorithm,
        int quantum = 2,
        QString* error = nullptr);
};
