#ifndef CONNECTIONSTATISTICSCHART_H
#define CONNECTIONSTATISTICSCHART_H

#include <QObject>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QTimer>
#include <QGraphicsItem>
#include <QWindow>
#include <QDateTime>
#include <list>
#include <vector>

#include "movingaverage.h"
#include "backends/qvdbackend.h"

QT_CHARTS_USE_NAMESPACE

class ConnectionStatisticsChart : public QChart
{
    Q_OBJECT
public:

    ConnectionStatisticsChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags{});
    virtual ~ConnectionStatisticsChart();

    void connectBackend(QVDBackend *backend);
    void disconnectBackend();
public slots:
    void updateChart();

private slots:
    void backendTraffic(int64_t in, int64_t out);


private:
    QTimer m_timer;
    QLineSeries *m_input_series;
    QLineSeries *m_output_series;
    QLineSeries *m_center_series;

    QAreaSeries *m_input_area;
    QAreaSeries *m_output_area;


    QStringList m_titles;
    QDateTimeAxis *m_axis_time;
    QValueAxis *m_axis_bandwidth;

    qreal m_step;
    qreal m_x;
    qreal m_y;

    int m_update_interval{100};

    QDateTime m_start_time;
    QVDBackend *m_backend{nullptr};
    MovingAverage m_in_avg{100};
    MovingAverage m_out_avg{100};

    qreal m_max_bw{0};
    bool m_stacked{false};

    int m_graph_seconds{30};
    std::vector<double> m_values;
    std::vector<double>::size_type m_value_pos{0};
};

#endif // CONNECTIONSTATISTICSCHART_H
