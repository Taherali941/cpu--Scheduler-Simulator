#pragma once

#include "scheduler.h"
#include <QWidget>

class GanttWidget : public QWidget {
    Q_OBJECT
public:
    explicit GanttWidget(QWidget* parent = nullptr);
    void setTimeline(const QVector<GanttSegment>& timeline);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize minimumSizeHint() const override;

private:
    QVector<GanttSegment> m_timeline;
};
