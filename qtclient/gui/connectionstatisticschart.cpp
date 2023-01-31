#include "connectionstatisticschart.h"
#include <algorithm>
#include <cmath>

ConnectionStatisticsChart::ConnectionStatisticsChart(QGraphicsItem *parent, Qt::WindowFlags flags)
    : QChart(QChart::ChartTypeCartesian, parent, flags),
      m_input_series(nullptr),
      m_output_series(nullptr),
      m_center_series(nullptr),
      m_input_area(nullptr),
      m_output_area(nullptr),
      m_axis_time(new QDateTimeAxis()),
      m_axis_bandwidth(new QValueAxis()),
      m_step(0),
      m_x(5),
      m_y(1)

{
    connect(&m_timer, &QTimer::timeout, this, &ConnectionStatisticsChart::updateChart);


    m_input_series = new QLineSeries(this);
    m_output_series = new QLineSeries(this);
    m_center_series = new QLineSeries(this);

    if (m_stacked) {
        m_input_area = new QAreaSeries(m_input_series, m_center_series);
        m_output_area = new QAreaSeries(m_center_series, m_output_series);
    } else {
        m_input_area = new QAreaSeries(m_input_series, m_center_series);
        m_output_area = new QAreaSeries(m_center_series, m_output_series);
    }

    m_input_series->setName("Download");
    m_output_series->setName("Upload");

    m_input_area->setName("Download");
    m_output_area->setName("Upload");

    QPen input_pen(Qt::red);
    QPen output_pen(Qt::blue);
//    QPen center_pen(Qt::black);

    input_pen.setWidth(1);
    output_pen.setWidth(1);



//    center_pen.setWidth(1);

    m_input_series->setPen(input_pen);
    m_output_series->setPen(output_pen);

    m_input_area->setPen(input_pen);
    m_output_area->setPen(output_pen);

//    m_center_series->setPen(center_pen);

    addSeries(m_input_area);
    addSeries(m_output_area);
//    addSeries(m_center_series);

    m_axis_time->setFormat("h:mm:ss");
    m_axis_bandwidth->setLabelFormat("%d");

    addAxis(m_axis_time, Qt::AlignBottom);
    addAxis(m_axis_bandwidth, Qt::AlignLeft);

    m_input_series->attachAxis(m_axis_time);
    m_input_series->attachAxis(m_axis_bandwidth);

    m_output_series->attachAxis(m_axis_time);
    m_output_series->attachAxis(m_axis_bandwidth);

    m_center_series->attachAxis(m_axis_time);
    m_center_series->attachAxis(m_axis_bandwidth);

    m_input_area->attachAxis(m_axis_time);
    m_input_area->attachAxis(m_axis_bandwidth);

    m_output_area->attachAxis(m_axis_time);
    m_output_area->attachAxis(m_axis_bandwidth);


    m_axis_time->setTickCount(6);
    m_axis_time->setTitleText("Time");


    m_axis_bandwidth->setRange(-16,16);
    m_axis_bandwidth->setTitleText("KiB/s");
    m_axis_bandwidth->setTickCount(10);



}

ConnectionStatisticsChart::~ConnectionStatisticsChart()
{

}

void ConnectionStatisticsChart::connectBackend(QVDBackend *backend)
{
    m_backend = backend;
    connect(m_backend, &QVDBackend::totalTrafficIncrement, this, &ConnectionStatisticsChart::backendTraffic);

    m_values.resize(m_graph_seconds * (1000.0/m_update_interval), 0);

    m_start_time = QDateTime::currentDateTimeUtc();
    m_in_avg.setInterval(m_update_interval * 5);
    m_out_avg.setInterval(m_update_interval * 5);
    m_timer.start(m_update_interval);

    m_axis_time->setRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addSecs(m_graph_seconds));

}

void ConnectionStatisticsChart::disconnectBackend()
{
    if (m_backend) {
        disconnect(m_backend, &QVDBackend::totalTrafficIncrement, this, &ConnectionStatisticsChart::backendTraffic);
    }

    m_timer.stop();
    m_backend = nullptr;
}

void ConnectionStatisticsChart::updateChart()
{
    //qreal seconds =  m_start_time.msecsTo(QDateTime::currentDateTimeUtc()) / 1000.0;
    QDateTime now = QDateTime::currentDateTime();

    qreal msecs = now.toMSecsSinceEpoch();

    qreal in_bw = m_in_avg.getAverage() / 1024;
    qreal out_bw = m_out_avg.getAverage() / 1024;

    m_input_series->append(msecs, in_bw);
    m_output_series->append(msecs, -out_bw);
    m_center_series->append(msecs, 0);

    while(m_input_series->count() > m_max_history_length)
        m_input_series->remove(0);

    while(m_output_series->count() > m_max_history_length)
        m_output_series->remove(0);

    while(m_center_series->count() > m_max_history_length)
        m_center_series->remove(0);



    m_values[m_value_pos++] = std::max(in_bw, out_bw);
    if ( m_value_pos >= m_values.size() ) {
        m_value_pos = 0;
    }

    m_max_bw = std::max(*std::max_element(m_values.begin(), m_values.end()), 32.0);

    double bw_log = std::log2(m_max_bw);
    double rounded_max = std::pow(2, std::ceil(bw_log));

    m_axis_bandwidth->setRange(-rounded_max, rounded_max);
    m_axis_time->setRange( now.addSecs(-m_graph_seconds), now );
}

void ConnectionStatisticsChart::backendTraffic(int64_t in, int64_t out)
{
    m_in_avg.add(in);
    m_out_avg.add(out);
}

