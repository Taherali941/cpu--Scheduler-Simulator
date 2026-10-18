#pragma once

#include "scheduler.h"
#include "ganttwidget.h"

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void addProcess();
    void removeSelectedProcess();
    void runSimulation();
    void loadSample();

private:
    QVector<ProcessInput> readProcesses(QString* error) const;
    void buildUi();
    void setStatus(const QString& text, bool error = false);

    QTableWidget* m_inputTable = nullptr;
    QTableWidget* m_statsTable = nullptr;
    QComboBox* m_algorithm = nullptr;
    QComboBox* m_priorityMode = nullptr;
    QSpinBox* m_quantum = nullptr;
    GanttWidget* m_gantt = nullptr;
    QLabel* m_status = nullptr;
    QLabel* m_avgWaiting = nullptr;
    QLabel* m_avgTurnaround = nullptr;
    QLabel* m_avgResponse = nullptr;
};
