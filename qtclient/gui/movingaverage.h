#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <QObject>
#include <QList>
#include <QDateTime>

class MovingAverage
{
public:
    MovingAverage(int milliseconds = 1000);

    void add(int64_t data);
    double getAverage();
    void clear() { m_data.clear(); }
    int interval() const { return m_max_age_msecs; }
    void setInterval(int msecs) { m_max_age_msecs = msecs; }

private:

    struct Entry {
        QDateTime timestamp;
        int64_t   data;
    };

    void trimData(const QDateTime &now);
    QList<Entry> m_data;
    int      m_max_age_msecs{1000};

};

#endif // MOVINGAVERAGE_H
