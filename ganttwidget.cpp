#include "ganttwidget.h"

#include <QPainter>
#include <QFontMetrics>
#include <algorithm>

GanttWidget::GanttWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(150);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
}

void GanttWidget::setTimeline(const QVector<GanttSegment>& timeline) {
    m_timeline = timeline;
    update();
}

QSize GanttWidget::minimumSizeHint() const {
    return QSize(480, 170);
}

void GanttWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), palette().base());

    if (m_timeline.isEmpty()) {
        p.setPen(palette().mid().color());
        p.drawText(rect(), Qt::AlignCenter, "Run a scheduling algorithm to view the Gantt chart");
        return;
    }

    const int left = 28;
    const int right = 28;
    const int top = 38;
    const int barHeight = 48;
    const int available = std::max(1, width() - left - right);
    const int totalTime = std::max(1, m_timeline.last().end - m_timeline.first().start);
    const double scale = static_cast<double>(available) / totalTime;

    const QVector<QColor> colors = {
        QColor("#2563eb"), QColor("#0d9488"), QColor("#9333ea"),
        QColor("#d97706"), QColor("#db2777"), QColor("#0891b2"),
        QColor("#4f46e5"), QColor("#65a30d")
    };

    QFont labelFont = font();
    labelFont.setPointSize(9);
    p.setFont(labelFont);

    for (int i = 0; i < m_timeline.size(); ++i) {
        const auto& s = m_timeline[i];
        const int x1 = left + qRound((s.start - m_timeline.first().start) * scale);
        const int x2 = left + qRound((s.end - m_timeline.first().start) * scale);
        QRect r(x1, top, std::max(1, x2 - x1), barHeight);
        QColor fill;
        if (s.pid == "IDLE") fill = QColor("#9ca3af");
        else {
            uint hash = qHash(s.pid);
            fill = colors[hash % static_cast<uint>(colors.size())];
        }
        p.setPen(QPen(fill.darker(115), 1));
        p.setBrush(fill);
        p.drawRoundedRect(r, 4, 4);

        p.setPen(Qt::white);
        if (r.width() >= 28) p.drawText(r, Qt::AlignCenter, s.pid);

        p.setPen(palette().text().color());
        p.drawLine(x1, top + barHeight, x1, top + barHeight + 7);
        p.drawText(QRect(x1 - 18, top + barHeight + 8, 40, 20),
                   Qt::AlignHCenter | Qt::AlignTop, QString::number(s.start));
    }

    const auto& last = m_timeline.last();
    const int finalX = left + qRound((last.end - m_timeline.first().start) * scale);
    p.setPen(palette().text().color());
    p.drawLine(finalX, top + barHeight, finalX, top + barHeight + 7);
    p.drawText(QRect(finalX - 18, top + barHeight + 8, 40, 20),
               Qt::AlignHCenter | Qt::AlignTop, QString::number(last.end));

    p.setPen(palette().mid().color());
    p.drawText(QRect(left, 8, available, 22), Qt::AlignLeft | Qt::AlignVCenter,
               QString("Execution timeline  •  Total elapsed time: %1").arg(last.end - m_timeline.first().start));
}
