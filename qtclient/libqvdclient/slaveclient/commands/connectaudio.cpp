#include "connectaudio.h"
#include <QUuid>
#include <QFile>

ConnectAudio::ConnectAudio(quint16 port)
{
    // We deal with two pulseaudio instances, our own, and the system one.
    //
    // Our own instance is shipped with the client.
    // * On Linux, it acts as a layer for decompressing audio and forwards to the system's PA.
    // * On OSX and Windows it talks to CoreAudio or Win32 audio.
    //
    // We always need this instance.
    QObject::connect(&m_pulse, &PulseAudio::started, this, &ConnectAudio::pulseStarted );
    QObject::connect(&m_pulse, &PulseAudio::stopped, this, &ConnectAudio::pulseStopped );
    QObject::connect(&m_pulse, &PulseAudio::error  , this, &ConnectAudio::pulseError );

#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
    // Only on Linux systems, we also connect to the system's PulseAudio so that we can ask
    // it to load modules.
    //
    // We connect both pulseaudio instances to the same handlers, since it we don't care to distinguish them,
    // all that is important is that we need to set things up once both are up and running.
    QObject::connect(&m_system_pulse, &PulseAudio::started, this, &ConnectAudio::pulseStarted );
    QObject::connect(&m_system_pulse, &PulseAudio::stopped, this, &ConnectAudio::pulseStopped );
    QObject::connect(&m_system_pulse, &PulseAudio::error  , this, &ConnectAudio::pulseError );

    m_pulse.setPulseaudioStatePath("/tmp/qvdpulsestate");
    m_secondary_audio_port = 12345;
#endif

    setAudioPort(port);

}

ConnectAudio::~ConnectAudio()
{
    stop();
}

void ConnectAudio::run()
{
    qInfo() << "Starting pulseaudio";
#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
    m_system_pulse.connectExisting();
#else
    QUuid random_id = QUuid::createUuid();
    QString random_filename = qgetenv("TEMP") + "\\pulseaudio-" + random_id.toString(QUuid::WithoutBraces) + ".pa";

    qDebug() << "Writing pulseaudio config to " << random_filename;

    QFile pa_config(random_filename);
    pa_config.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream pa_stream(&pa_config);

    pa_stream << "# Generated by QVD Client" << Qt::endl;
    pa_stream << QString("load-module module-native-protocol-tcp auth-anonymous=1 listen=127.0.0.1 port=%1").arg(audioPort()) << Qt::endl;
    //pa_stream << QString("load-module module-cli-protocol-tcp listen=127.0.0.1 port=%1").arg(commandPort()) << Qt::endl();

    if (microphone()) {
        qDebug() << "Speakers and microphone enabled";
        pa_stream << "load-module module-waveout\n";           // This may not work if we can't record
        pa_stream << "load-module module-waveout record=0\n";  // Fallback
    } else {
        qDebug() << "Speakers only enabled";
        pa_stream << "load-module module-waveout record=0\n";
    }

    pa_stream.flush();
    pa_config.close();

    m_pulse.setQvdPulseaudioConfig(random_filename);
    m_temp_files.append(random_filename);
#endif
    //m_pulse.set
    m_pulse.start();
}

void ConnectAudio::stop()
{
    qInfo() << "Stopping";
    m_system_pulse.stop();
    m_pulse.stop();
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
    qDebug() << "Cleaning up";

    for(auto& filename : m_temp_files) {
        qDebug() << "Removing " << filename;
        QFile::remove(filename);
    }
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
