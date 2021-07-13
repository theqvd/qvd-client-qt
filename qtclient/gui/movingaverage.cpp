#include "movingaverage.h"
#include <QDebug>



MovingAverage::MovingAverage(int milliseconds) : m_data(), m_max_age_msecs(milliseconds)
{

}

void MovingAverage::add(int64_t data)
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    m_data.append({ now, data });
    trimData(now);
}

double MovingAverage::getAverage()
{
    trimData(QDateTime::currentDateTimeUtc());
    double total = 0;
    for(const auto &item : m_data) {
        total += item.data;
    }

    //qInfo() << "Total: " << total << " from " << m_data.length() << "items";
    return total / ((double)m_max_age_msecs / 1000.0);
}

void MovingAverage::trimData(const QDateTime &now)
{
    QDateTime oldest = now.addMSecs(-m_max_age_msecs);

    while(!m_data.isEmpty() && m_data[0].timestamp < oldest) {
        m_data.removeFirst();
    }
}
