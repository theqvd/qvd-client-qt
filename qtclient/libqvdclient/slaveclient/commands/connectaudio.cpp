#include "connectaudio.h"

ConnectAudio::ConnectAudio(quint16 port)
{
    QObject::connect(&m_pulse, &PulseAudio::started, this, &ConnectAudio::pulseStarted );
    QObject::connect(&m_pulse, &PulseAudio::stopped, this, &ConnectAudio::pulseStopped );
    QObject::connect(&m_pulse, &PulseAudio::error  , this, &ConnectAudio::pulseError );

    // We connect both pulseaudio instances to the same handlers, since it we don't care to distinguish them,
    // all that is important is that we need to set things up once both are up and running.
    QObject::connect(&m_system_pulse, &PulseAudio::started, this, &ConnectAudio::pulseStarted );
    QObject::connect(&m_system_pulse, &PulseAudio::stopped, this, &ConnectAudio::pulseStopped );
    QObject::connect(&m_system_pulse, &PulseAudio::error  , this, &ConnectAudio::pulseError );


    m_pulse.setPulseaudioStatePath("/tmp/qvdpulsestate");
    m_secondary_audio_port = 12345;
    setAudioPort(port);
}

void ConnectAudio::run()
{
    qInfo() << "Starting pulseaudio";
    m_system_pulse.connectExisting();
    m_pulse.start();
}

void ConnectAudio::pulseStarted()
{
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
