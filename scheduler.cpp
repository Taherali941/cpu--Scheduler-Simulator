#include "scheduler.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <QHash>

namespace {
struct WorkProcess {
    ProcessInput input;
    int inputIndex = 0;
    int remaining = 0;
    int firstStart = -1;
    int completion = -1;
};

void appendSegment(QVector<GanttSegment>& timeline, const QString& pid, int start, int end) {
    if (end <= start) return;
    if (!timeline.isEmpty() && timeline.last().pid == pid && timeline.last().end == start) {
        timeline.last().end = end;
    } else {
        timeline.push_back({pid, start, end});
    }
}

bool validate(const QVector<ProcessInput>& processes, Algorithm algorithm, int quantum, QString* error) {
    auto fail = [&](const QString& message) {
        if (error) *error = message;
        return false;
    };
    if (processes.isEmpty()) return fail("Add at least one process before simulating.");
    if (algorithm == Algorithm::RoundRobin && quantum <= 0)
        return fail("Round Robin time quantum must be greater than zero.");

    QHash<QString, bool> seen;
    for (const auto& p : processes) {
        const QString id = p.pid.trimmed();
        if (id.isEmpty()) return fail("Every process must have a Process ID.");
        if (seen.contains(id)) return fail("Process IDs must be unique. Duplicate ID: " + id);
        seen.insert(id, true);
        if (p.arrival < 0) return fail("Arrival time cannot be negative (" + id + ").");
        if (p.burst <= 0) return fail("Burst time must be greater than zero (" + id + ").");
    }
    return true;
}

QVector<WorkProcess> makeWork(const QVector<ProcessInput>& processes) {
    QVector<WorkProcess> work;
    work.reserve(processes.size());
    for (int i = 0; i < processes.size(); ++i)
        work.push_back({processes[i], i, processes[i].burst, -1, -1});
    return work;
}

ScheduleResult finish(const QVector<WorkProcess>& work, const QVector<GanttSegment>& timeline) {
    ScheduleResult result;
    result.timeline = timeline;
    result.stats.resize(work.size());
    double sumW = 0, sumT = 0, sumR = 0;
    for (const auto& w : work) {
        ProcessStats s;
        s.pid = w.input.pid;
        s.arrival = w.input.arrival;
        s.burst = w.input.burst;
        s.priority = w.input.priority;
        s.completion = w.completion;
        s.turnaround = s.completion - s.arrival;
        s.waiting = s.turnaround - s.burst;
        s.response = w.firstStart - s.arrival;
        result.stats[w.inputIndex] = s;
        sumW += s.waiting;
        sumT += s.turnaround;
        sumR += s.response;
    }
    const double n = static_cast<double>(work.size());
    result.averageWaiting = sumW / n;
    result.averageTurnaround = sumT / n;
    result.averageResponse = sumR / n;
    return result;
}
} // namespace

std::optional<ScheduleResult> Scheduler::run(
    const QVector<ProcessInput>& processes, Algorithm algorithm, int quantum, QString* error) {
    if (!validate(processes, algorithm, quantum, error)) return std::nullopt;

    QVector<WorkProcess> work = makeWork(processes);
    QVector<GanttSegment> timeline;
    const int n = work.size();
    int completed = 0;
    int time = 0;

    if (algorithm == Algorithm::FCFS || algorithm == Algorithm::SJF ||
        algorithm == Algorithm::PriorityNonPreemptive) {
        while (completed < n) {
            int chosen = -1;
            for (int i = 0; i < n; ++i) {
                if (work[i].completion != -1 || work[i].input.arrival > time) continue;
                if (chosen == -1) { chosen = i; continue; }

                const auto& a = work[i];
                const auto& b = work[chosen];
                bool better = false;
                if (algorithm == Algorithm::FCFS) {
                    better = a.input.arrival < b.input.arrival ||
                        (a.input.arrival == b.input.arrival && a.inputIndex < b.inputIndex);
                } else if (algorithm == Algorithm::SJF) {
                    better = a.input.burst < b.input.burst ||
                        (a.input.burst == b.input.burst &&
                         (a.input.arrival < b.input.arrival ||
                          (a.input.arrival == b.input.arrival && a.inputIndex < b.inputIndex)));
                } else {
                    better = a.input.priority < b.input.priority ||
                        (a.input.priority == b.input.priority &&
                         (a.input.arrival < b.input.arrival ||
                          (a.input.arrival == b.input.arrival && a.inputIndex < b.inputIndex)));
                }
                if (better) chosen = i;
            }

            if (chosen == -1) {
                int nextArrival = std::numeric_limits<int>::max();
                for (const auto& w : work)
                    if (w.completion == -1) nextArrival = std::min(nextArrival, w.input.arrival);
                appendSegment(timeline, "IDLE", time, nextArrival);
                time = nextArrival;
                continue;
            }

            auto& p = work[chosen];
            if (p.firstStart == -1) p.firstStart = time;
            appendSegment(timeline, p.input.pid, time, time + p.remaining);
            time += p.remaining;
            p.remaining = 0;
            p.completion = time;
            ++completed;
        }
        return finish(work, timeline);
    }

    if (algorithm == Algorithm::SRTF || algorithm == Algorithm::PriorityPreemptive) {
        int running = -1;
        while (completed < n) {
            int chosen = -1;
            for (int i = 0; i < n; ++i) {
                if (work[i].completion != -1 || work[i].input.arrival > time) continue;
                if (chosen == -1) { chosen = i; continue; }
                const auto& a = work[i];
                const auto& b = work[chosen];
                int av = algorithm == Algorithm::SRTF ? a.remaining : a.input.priority;
                int bv = algorithm == Algorithm::SRTF ? b.remaining : b.input.priority;
                bool better = av < bv;
                if (av == bv) {
                    if (i == running) better = true; // preserve current process on ties
                    else if (chosen != running &&
                             (a.input.arrival < b.input.arrival ||
                              (a.input.arrival == b.input.arrival && a.inputIndex < b.inputIndex)))
                        better = true;
                }
                if (better) chosen = i;
            }

            if (chosen == -1) {
                int nextArrival = std::numeric_limits<int>::max();
                for (const auto& w : work)
                    if (w.completion == -1) nextArrival = std::min(nextArrival, w.input.arrival);
                appendSegment(timeline, "IDLE", time, nextArrival);
                time = nextArrival;
                running = -1;
                continue;
            }

            auto& p = work[chosen];
            if (p.firstStart == -1) p.firstStart = time;
            appendSegment(timeline, p.input.pid, time, time + 1);
            --p.remaining;
            ++time;
            running = chosen;
            if (p.remaining == 0) {
                p.completion = time;
                ++completed;
                running = -1;
            }
        }
        return finish(work, timeline);
    }

    // Round Robin: ready queue ordered by arrival; arrivals during a time slice
    // are enqueued before the preempted process is re-enqueued.
    QVector<int> arrivalOrder;
    for (int i = 0; i < n; ++i) arrivalOrder.push_back(i);
    std::stable_sort(arrivalOrder.begin(), arrivalOrder.end(), [&](int a, int b) {
        if (work[a].input.arrival != work[b].input.arrival)
            return work[a].input.arrival < work[b].input.arrival;
        return work[a].inputIndex < work[b].inputIndex;
    });

    std::deque<int> ready;
    int next = 0;
    while (completed < n) {
        while (next < n && work[arrivalOrder[next]].input.arrival <= time)
            ready.push_back(arrivalOrder[next++]);

        if (ready.empty()) {
            if (next < n) {
                const int nextArrival = work[arrivalOrder[next]].input.arrival;
                appendSegment(timeline, "IDLE", time, nextArrival);
                time = nextArrival;
                continue;
            }
            break;
        }

        const int idx = ready.front();
        ready.pop_front();
        auto& p = work[idx];
        if (p.firstStart == -1) p.firstStart = time;
        const int runFor = std::min(quantum, p.remaining);
        const int endTime = time + runFor;
        appendSegment(timeline, p.input.pid, time, endTime);
        p.remaining -= runFor;
        time = endTime;

        while (next < n && work[arrivalOrder[next]].input.arrival <= time)
            ready.push_back(arrivalOrder[next++]);

        if (p.remaining > 0) ready.push_back(idx);
        else {
            p.completion = time;
            ++completed;
        }
    }
    return finish(work, timeline);
}
