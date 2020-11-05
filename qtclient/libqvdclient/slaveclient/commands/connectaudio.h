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

    virtual ~ConnectAudio() = default;

    virtual void run();
    quint16 audioPort() const;
    void setAudioPort(const quint16 &audio_port);

    quint16 secondaryAudioPort() const;
    void setSecondaryAudioPort(const quint16 &secondary_audio_port);

public slots:
    void pulseStarted();
    void pulseStopped();
    void pulseError(const QString &err);
private:
    PulseAudio m_system_pulse;
    PulseAudio m_pulse;

    quint16 m_audio_port = 4731;
    quint16 m_secondary_audio_port = 0;

};

QDebug operator<< (QDebug d, const ConnectAudio &audio);

#endif // CONNECTAUDIO_H
