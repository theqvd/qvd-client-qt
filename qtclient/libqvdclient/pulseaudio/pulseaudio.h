#ifndef PULSEAUDIOBASE_H
#define PULSEAUDIOBASE_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QLocalSocket>
#include <QVersionNumber>
#include "helpers/linebuffer.h"
#include "qvdclient_global.h"

/**
 * @brief Manages a PulseAudio instance
 *
 * This class can work in two modes: either starting a PulseAudio instance on its own, or
 * connecting to an existing one.
 *
 * If a PulseAudio instance is started with start(), it will manage the process' lifetime,
 * and monitor its log output to determine exactly when interesting events happen.
 */
class LIBQVDCLIENT_EXPORT PulseAudio : public QObject
{
    Q_OBJECT

public:
    PulseAudio();
    virtual ~PulseAudio();

    /**
     * @brief Connect to existing PulseAudio instance
     * @param pulse_dir Directory where PulseAudio's 'pid' file can be found.
     */
    virtual void connectExisting(const QString &pulse_dir = "");

    /**
     * @brief Start new PulseAudio instance
     *
     * This will launch a new PulseAudio and control it during the class' lifetime.
     */
    virtual void start();


    /**
     * @brief Activate PulseAudio's communication socket
     */
    virtual void activateSocket();

    virtual QString sendCommand(const QString &command);


    /**
     * @brief Stop this PulseAudio instance
     */
    virtual void stop();

    /**
     * @brief Check whether QVD's custom PulseAudio is installed
     * @return True if installed
     */
    virtual bool isQvdPulseaudioInstalled();

    /**
     * @brief Check whether the Opus codec is supported
     * @return True if supported
     */
    virtual bool isOpusSupported();

    /**
     * @brief Check whether PulseAudio is running
     * @return True if running
     */
    virtual bool isRunning();

    /**
     * @brief Returns the extension for a PulseAudio module for this system
     * @return The extension of a PulseAudio module on this OS (eg, ".so")
     */
    virtual QString getPulseModuleExtension();

    virtual QString getPulseSocketPath();

    /**
     * @brief Returns the path where PulseAudio modules can be found on this system
     * @return Path
     */
    virtual QString getPulseModulePath();

    QString getPulseaudioHome() const;
    void setPulseaudioHome(const QString &pulseaudio_home);

    QString getPulseaudioStatePath() const;
    void setPulseaudioStatePath(const QString &pulseaudio_state_path);

    QString getQvdPulseaudioPath() const;
    void setQvdPulseaudioPath(const QString &qvd_pulseaudio_path);

    QString getQvdPulseaudioConfig() const;
    void setQvdPulseaudioConfig(const QString &qvd_pulseaudio_config);

    QVersionNumber getVersion() const { return m_version; }

signals:
    void started();
    void stopped();
    void error(const QString &error);

public slots:
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processStarted();
    void processReadyReadOutput();
    void processReadyReadError();

    void connectionTimerTick();
    void socketConnected();
    void socketDisconnected();
    void socketError(QLocalSocket::LocalSocketError err);


protected:
    QStringList getLines(QString &buffer, const QString &new_data);

    const QString PULSEAUDIO_PROMPT{">>> "};

    bool m_using_existing_process = false;
    int m_pid = 0;

    QTimer       m_connection_timer;
    QLocalSocket m_pulse_socket;

    QString m_qvd_pulseaudio_path = "/usr/lib/qvd/bin/pulseaudio";
    QString m_qvd_pulseaudio_home{""};
    QString m_qvd_pulseaudio_state_path{""};
    QString m_qvd_pulseaudio_config{""};

    QVersionNumber m_version;

    LineBuffer m_pulse_out_buf;
    LineBuffer m_pulse_err_buf;

    // QProcess may emit events during destruction, such as handling the last messages emitted by the process.
    // The last declared member will be the first to be destroyed, so QProcess has to be last here, ensuring
    // the rest of the class is still there to deal with any last-time events.
    QProcess     m_process;
};

#endif // PULSEAUDIOBASE_H
