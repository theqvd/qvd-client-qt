#ifndef CONNECTAUDIO_H
#define CONNECTAUDIO_H

#include <QObject>
#include "qvdslavecommand.h"
#include "pulseaudio/pulseaudio.h"

class ConnectAudio : public QVDSlaveCommand
{
    Q_OBJECT
public:

    /**
     * @brief Connect audio
     * @param audio_port Port to which nxproxy should connect
     */
    ConnectAudio(quint16 audio_port);

    virtual ~ConnectAudio();

    virtual void run();
    virtual void stop();
    quint16 audioPort() const;
    void setAudioPort(const quint16 &audio_port);

    quint16 secondaryAudioPort() const;
    void setSecondaryAudioPort(const quint16 &secondary_audio_port);

    bool microphone() const { return m_microphone; }
    void setMicrophone(bool microphone) { m_microphone = microphone; }

public slots:
    void pulseStarted();
    void pulseStopped();
    void pulseError(const QString &err);
private:
    PulseAudio m_system_pulse;
    PulseAudio m_pulse;
    QList<QString> m_temp_files;

    quint16 m_audio_port = 4731;
    quint16 m_secondary_audio_port = 0;
    bool m_microphone = false;

};

QDebug operator<< (QDebug d, const ConnectAudio &audio);

#endif // CONNECTAUDIO_H
