#include "mainwindow.h"

#include <QApplication>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSplitter>
#include <QDoubleSpinBox>
#include <QAbstractItemView>
#include <QIntValidator>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    buildUi();
    loadSample();
    resize(1120, 820);
}

void MainWindow::buildUi() {
    setWindowTitle("CPU Scheduling Simulator");
    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(18, 16, 18, 16);
    root->setSpacing(12);

    auto* title = new QLabel("CPU Scheduling Simulator");
    QFont titleFont = title->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);
    auto* subtitle = new QLabel("Visualize CPU scheduling algorithms, Gantt charts, and process performance.");
    subtitle->setStyleSheet("color: #64748b;");
    root->addWidget(title);
    root->addWidget(subtitle);

    auto* controls = new QGroupBox("Simulation settings");
    auto* controlLayout = new QHBoxLayout(controls);
    m_algorithm = new QComboBox;
    m_algorithm->addItem("FCFS (First Come, First Served)");
    m_algorithm->addItem("SJF (Shortest Job First)");
    m_algorithm->addItem("SRTF (Shortest Remaining Time First)");
    m_algorithm->addItem("Priority Scheduling");
    m_algorithm->addItem("Round Robin");
    m_priorityMode = new QComboBox;
    m_priorityMode->addItem("Non-preemptive");
    m_priorityMode->addItem("Preemptive");
    m_quantum = new QSpinBox;
    m_quantum->setRange(1, 100000);
    m_quantum->setValue(2);
    m_quantum->setSuffix(" units");
    controlLayout->addWidget(new QLabel("Algorithm:"));
    controlLayout->addWidget(m_algorithm, 3);
    controlLayout->addWidget(new QLabel("Priority mode:"));
    controlLayout->addWidget(m_priorityMode, 1);
    controlLayout->addWidget(new QLabel("RR quantum:"));
    controlLayout->addWidget(m_quantum, 1);
    root->addWidget(controls);

    connect(m_algorithm, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_priorityMode->setEnabled(index == 3);
        m_quantum->setEnabled(index == 4);
    });
    m_priorityMode->setEnabled(false);
    m_quantum->setEnabled(false);

    auto* split = new QSplitter(Qt::Horizontal);
    auto* inputBox = new QGroupBox("Process input");
    auto* inputLayout = new QVBoxLayout(inputBox);
    m_inputTable = new QTableWidget(0, 4);
    m_inputTable->setHorizontalHeaderLabels({"Process ID", "Arrival Time", "Burst Time", "Priority"});
    m_inputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_inputTable->verticalHeader()->setVisible(false);
    m_inputTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_inputTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_inputTable->setAlternatingRowColors(true);
    inputLayout->addWidget(m_inputTable);

    auto* inputButtons = new QHBoxLayout;
    auto* add = new QPushButton("+ Add process");
    auto* remove = new QPushButton("Remove selected");
    auto* sample = new QPushButton("Load sample");
    inputButtons->addWidget(add);
    inputButtons->addWidget(remove);
    inputButtons->addWidget(sample);
    inputLayout->addLayout(inputButtons);
    connect(add, &QPushButton::clicked, this, &MainWindow::addProcess);
    connect(remove, &QPushButton::clicked, this, &MainWindow::removeSelectedProcess);
    connect(sample, &QPushButton::clicked, this, &MainWindow::loadSample);

    auto* resultsBox = new QGroupBox("Process statistics");
    auto* resultsLayout = new QVBoxLayout(resultsBox);
    m_statsTable = new QTableWidget(0, 8);
    m_statsTable->setHorizontalHeaderLabels({"PID", "Arrival", "Burst", "Priority", "Completion", "Turnaround", "Waiting", "Response"});
    m_statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_statsTable->horizontalHeader()->setStretchLastSection(true);
    m_statsTable->verticalHeader()->setVisible(false);
    m_statsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_statsTable->setAlternatingRowColors(true);
    resultsLayout->addWidget(m_statsTable);
    split->addWidget(inputBox);
    split->addWidget(resultsBox);
    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 2);
    root->addWidget(split, 3);

    auto* ganttBox = new QGroupBox("Gantt chart");
    auto* ganttLayout = new QVBoxLayout(ganttBox);
    m_gantt = new GanttWidget;
    m_gantt->setMinimumHeight(150);
    ganttLayout->addWidget(m_gantt);
    root->addWidget(ganttBox, 1);

    auto* bottom = new QHBoxLayout;
    m_avgWaiting = new QLabel("Average waiting: —");
    m_avgTurnaround = new QLabel("Average turnaround: —");
    m_avgResponse = new QLabel("Average response: —");
    bottom->addWidget(m_avgWaiting);
    bottom->addWidget(m_avgTurnaround);
    bottom->addWidget(m_avgResponse);
    bottom->addStretch();
    auto* run = new QPushButton("Run simulation");
    run->setDefault(true);
    run->setMinimumHeight(38);
    run->setStyleSheet("QPushButton { background:#2563eb; color:white; font-weight:600; padding:8px 18px; border-radius:6px; } QPushButton:hover { background:#1d4ed8; }");
    bottom->addWidget(run);
    connect(run, &QPushButton::clicked, this, &MainWindow::runSimulation);
    root->addLayout(bottom);

    m_status = new QLabel("Ready. Edit the sample processes or add your own.");
    m_status->setWordWrap(true);
    root->addWidget(m_status);

    setCentralWidget(central);
    setStyleSheet("QGroupBox { font-weight:600; border:1px solid #cbd5e1; border-radius:8px; margin-top:8px; padding-top:8px; } QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 5px; } QTableWidget { gridline-color:#e2e8f0; }");
}

void MainWindow::addProcess() {
    const int row = m_inputTable->rowCount();
    m_inputTable->insertRow(row);
    m_inputTable->setItem(row, 0, new QTableWidgetItem(QString("P%1").arg(row + 1)));
    m_inputTable->setItem(row, 1, new QTableWidgetItem("0"));
    m_inputTable->setItem(row, 2, new QTableWidgetItem("1"));
    m_inputTable->setItem(row, 3, new QTableWidgetItem("1"));
    m_inputTable->setCurrentCell(row, 0);
    m_inputTable->editItem(m_inputTable->item(row, 0));
}

void MainWindow::removeSelectedProcess() {
    const int row = m_inputTable->currentRow();
    if (row >= 0) m_inputTable->removeRow(row);
    else setStatus("Select a process row to remove.", true);
}

void MainWindow::loadSample() {
    const QVector<ProcessInput> sample = {
        {"P1", 0, 5, 2}, {"P2", 1, 3, 1}, {"P3", 2, 1, 3}
    };
    m_inputTable->setRowCount(0);
    for (const auto& p : sample) {
        int row = m_inputTable->rowCount();
        m_inputTable->insertRow(row);
        m_inputTable->setItem(row, 0, new QTableWidgetItem(p.pid));
        m_inputTable->setItem(row, 1, new QTableWidgetItem(QString::number(p.arrival)));
        m_inputTable->setItem(row, 2, new QTableWidgetItem(QString::number(p.burst)));
        m_inputTable->setItem(row, 3, new QTableWidgetItem(QString::number(p.priority)));
    }
    setStatus("Loaded sample data: P1, P2, and P3.");
}

QVector<ProcessInput> MainWindow::readProcesses(QString* error) const {
    QVector<ProcessInput> processes;
    for (int row = 0; row < m_inputTable->rowCount(); ++row) {
        for (int col = 0; col < 4; ++col) {
            if (!m_inputTable->item(row, col)) {
                if (error) *error = QString("Fill every cell in row %1.").arg(row + 1);
                return {};
            }
        }
        bool okArrival = false, okBurst = false, okPriority = false;
        const QString pid = m_inputTable->item(row, 0)->text().trimmed();
        const int arrival = m_inputTable->item(row, 1)->text().trimmed().toInt(&okArrival);
        const int burst = m_inputTable->item(row, 2)->text().trimmed().toInt(&okBurst);
        const int priority = m_inputTable->item(row, 3)->text().trimmed().toInt(&okPriority);
        if (pid.isEmpty() || !okArrival || !okBurst || !okPriority) {
            if (error) *error = QString("Row %1 contains an empty or invalid value. Use integer times and priority.").arg(row + 1);
            return {};
        }
        processes.push_back({pid, arrival, burst, priority});
    }
    return processes;
}

void MainWindow::setStatus(const QString& text, bool error) {
    m_status->setText(text);
    m_status->setStyleSheet(error ? "color:#b91c1c; font-weight:600;" : "color:#475569;");
}

void MainWindow::runSimulation() {
    QString inputError;
    const auto processes = readProcesses(&inputError);
    if (!inputError.isEmpty()) {
        setStatus(inputError, true);
        return;
    }

    Algorithm algorithm = Algorithm::FCFS;
    switch (m_algorithm->currentIndex()) {
    case 0: algorithm = Algorithm::FCFS; break;
    case 1: algorithm = Algorithm::SJF; break;
    case 2: algorithm = Algorithm::SRTF; break;
    case 3: algorithm = m_priorityMode->currentIndex() == 0
                            ? Algorithm::PriorityNonPreemptive : Algorithm::PriorityPreemptive; break;
    case 4: algorithm = Algorithm::RoundRobin; break;
    }

    QString error;
    const auto result = Scheduler::run(processes, algorithm, m_quantum->value(), &error);
    if (!result) {
        setStatus(error, true);
        return;
    }

    m_statsTable->setRowCount(result->stats.size());
    for (int row = 0; row < result->stats.size(); ++row) {
        const auto& s = result->stats[row];
        const QStringList values = {
            s.pid, QString::number(s.arrival), QString::number(s.burst), QString::number(s.priority),
            QString::number(s.completion), QString::number(s.turnaround), QString::number(s.waiting),
            QString::number(s.response)
        };
        for (int col = 0; col < values.size(); ++col)
            m_statsTable->setItem(row, col, new QTableWidgetItem(values[col]));
    }

    m_gantt->setTimeline(result->timeline);
    auto fmt = [](double value) { return QString::number(value, 'f', 2); };
    m_avgWaiting->setText("Average waiting: " + fmt(result->averageWaiting));
    m_avgTurnaround->setText("Average turnaround: " + fmt(result->averageTurnaround));
    m_avgResponse->setText("Average response: " + fmt(result->averageResponse));

    QString algorithmName = m_algorithm->currentText();
    if (algorithm == Algorithm::PriorityPreemptive) algorithmName += " (preemptive)";
    if (algorithm == Algorithm::PriorityNonPreemptive) algorithmName += " (non-preemptive)";
    if (algorithm == Algorithm::RoundRobin) algorithmName += QString(" (q=%1)").arg(m_quantum->value());
    setStatus(QString("Simulation completed: %1 processes scheduled across %2 timeline segments.")
              .arg(result->stats.size()).arg(result->timeline.size()));
}
