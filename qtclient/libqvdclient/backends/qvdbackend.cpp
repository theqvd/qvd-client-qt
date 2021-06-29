#include "qvdbackend.h"

QVDBackend::QVDBackend(QObject *parent) : QObject(parent)
{

}

QVDBackend::~QVDBackend()
{

}

QVDConnectionParameters QVDBackend::parameters() const
{
    return m_parameters;
}

void QVDBackend::setParameters(const QVDConnectionParameters &parameters)
{
    m_parameters = parameters;
}

int64_t QVDBackend::bytesRead()
{
    return 0;
}

int64_t QVDBackend::bytesWritten()
{
    return 0;
}

quint16 QVDBackend::cupsPort() const
{
    return m_cups_port;
}

void QVDBackend::setCupsPort(const quint16 &cups_port)
{
    m_cups_port = cups_port;
}

quint16 QVDBackend::slavePort() const
{
    return m_slave_port;
}

void QVDBackend::setSlavePort(const quint16 &slave_port)
{
    m_slave_port = slave_port;
}

quint16 QVDBackend::audioPort() const
{
    return m_audio_port;
}

void QVDBackend::setAudioPort(const quint16 &audio_port)
{
    m_audio_port = audio_port;
}

void QVDBackend::addStat(QVDBackend::NXChannel channel, int64_t in, int64_t out)
{
    if (!m_statistics.channelTraffic.contains(channel)) {
        m_statistics.channelTraffic.insert(channel, {in, out});
    } else {
        m_statistics.channelTraffic[channel].in  += in;
        m_statistics.channelTraffic[channel].out += out;
    }

    emit statisticsUpdated(m_statistics);
    emit channelStatIncrement(channel, in, out);
}
