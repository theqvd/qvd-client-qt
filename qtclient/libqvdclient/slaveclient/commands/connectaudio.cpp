#include "connectaudio.h"

ConnectAudio::ConnectAudio(quint16 port)
{
    QObject::connect(&m_pulse, &PulseAudio::started, this, &ConnectAudio::pulseStarted );
    QObject::connect(&m_pulse, &PulseAudio::stopped, this, &ConnectAudio::pulseStopped );
    QObject::connect(&m_pulse, &PulseAudio::error  , this, &ConnectAudio::pulseError );

#ifndef Q_OS_WIN
    // We connect both pulseaudio instances to the same handlers, since it we don't care to distinguish them,
    // all that is important is that we need to set things up once both are up and running.
    QObject::connect(&m_system_pulse, &PulseAudio::started, this, &ConnectAudio::pulseStarted );
    QObject::connect(&m_system_pulse, &PulseAudio::stopped, this, &ConnectAudio::pulseStopped );
    QObject::connect(&m_system_pulse, &PulseAudio::error  , this, &ConnectAudio::pulseError );

    m_pulse.setPulseaudioStatePath("/tmp/qvdpulsestate");
    m_secondary_audio_port = 12345;
    setAudioPort(port);
#endif

}

void ConnectAudio::run()
{
    qInfo() << "Starting pulseaudio";
#ifndef Q_OS_WIN
    m_system_pulse.connectExisting();
#endif
    m_pulse.start();
}

void ConnectAudio::pulseStarted()
{

#ifndef Q_OS_WIN
    if (!m_system_pulse.isRunning()) {
        qInfo() << "System pulse isn't connected yet, waiting";
        return;
    }

    if (!m_pulse.isRunning()) {
        qInfo() << "QVD pulse isn't connected yet, waiting";
        return;
    }

    qInfo() << "Both PA instances connected, setting up...";

    m_system_pulse.sendCommand(QString("load-module module-native-protocol-tcp auth-anonymous=1 listen=127.0.0.1 port=%1").arg(m_secondary_audio_port));

    m_pulse.sendCommand(QString("load-module module-native-protocol-tcp auth-anonymous=1 listen=127.0.0.1 port=%1").arg(m_audio_port));
    m_pulse.sendCommand(QString("load-module module-tunnel-sink-new sink_name=QVD server=tcp:127.0.0.1:%1 sink=@DEFAULT_SINK@").arg(m_secondary_audio_port));
#else
    qInfo() << "PulseAudio started";
#endif

    emit commandSuccessful();
}

void ConnectAudio::pulseStopped()
{

}

void ConnectAudio::pulseError(const QString &err)
{
    qCritical() << "Pulseaudio returned an error: " << err;
    emit commandFailed();
}


quint16 ConnectAudio::secondaryAudioPort() const
{
    return m_secondary_audio_port;
}

void ConnectAudio::setSecondaryAudioPort(const quint16 &secondary_audio_port)
{
    m_secondary_audio_port = secondary_audio_port;
}

quint16 ConnectAudio::audioPort() const
{
    return m_audio_port;
}

void ConnectAudio::setAudioPort(const quint16 &audio_port)
{
    m_audio_port = audio_port;
}

QDebug operator<<(QDebug d, const ConnectAudio &audio)
{
    d << "ConnectAudio(audioPort = " << audio.audioPort() << ")";
    return d;
}
