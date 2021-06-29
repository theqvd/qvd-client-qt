#ifndef CONNECTIONSTATISTICS_H
#define CONNECTIONSTATISTICS_H

#include <QDialog>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtCharts/QChartView>

#include "backends/qvdbackend.h"
#include "movingaverage.h"
#include "connectionstatisticschart.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class ConnectionStatistics;
}

class ConnectionStatistics : public QDialog
{
    Q_OBJECT

    const int UPDATE_INTERVAL = 100;

public:
    explicit ConnectionStatistics(QWidget *parent = nullptr);
    ~ConnectionStatistics();

    void connectToBackend(QVDBackend *backend);
    void disconnectBackend();

private:
    Ui::ConnectionStatistics *ui;
    ConnectionStatisticsChart *m_chart;
    QChartView *m_chart_view;
};

#endif // CONNECTIONSTATISTICS_H
