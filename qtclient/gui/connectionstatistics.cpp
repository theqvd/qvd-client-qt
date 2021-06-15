#include "connectionstatistics.h"
#include "ui_connectionstatistics.h"

#include <QtCharts/QChart>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

ConnectionStatistics::ConnectionStatistics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionStatistics)
{


    m_chart = new ConnectionStatisticsChart();
    m_chart->setTitle("Bandwidth usage");
   // m_chart->setAnimationOptions(QChart::AllAnimations);
    m_chart->setAnimationOptions(QChart::NoAnimation);


    m_chart_view = new QChartView(m_chart);
    m_chart_view->setRenderHint(QPainter::Antialiasing);



    ui->setupUi(this);
    ui->graphLayout->addWidget(m_chart_view);

  //  ui->graphFrame->ad
    //ui->graphFrame
}

ConnectionStatistics::~ConnectionStatistics()
{
    delete ui;
}

void ConnectionStatistics::connectToBackend(QVDBackend *backend)
{
    m_chart->connectBackend(backend);
}

void ConnectionStatistics::disconnectBackend()
{
    m_chart->disconnectBackend();
}


