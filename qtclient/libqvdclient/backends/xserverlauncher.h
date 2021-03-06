#ifndef XSERVERLAUNCHER_H
#define XSERVERLAUNCHER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QTcpSocket>

/**
 * @brief Launcher for X servers
 *
 * This class ensures an X server is up and running for NX (or anything else that might need it)
 * to connect to. It's OS specific.
 *
 * On \b{Windows}, it starts Vcxsrv and waits for it to come up and start accepting connections.
 *
 * On \b{Linux}, it doesn't do anything.
 *
 * \b{OSX} support is not implemented yet, but it will ensure XQuartz is up and running.
 *
 *
 */
class XServerLauncher : public QObject
{
    Q_OBJECT

public:
    XServerLauncher();
    ~XServerLauncher();

    /**
     * @brief Start the X server
     *
     * Emits the running() signal once it's up, or failed() if something has gone wrong.
     */
    void start();

    /**
     * @brief Stops the X server
     *
     * Emits the stopped() signal once it's down.
     */
    void stop();

    /**
     * @brief X11 display
     * @return The display number the X server is using
     */
    quint16 display() const { return m_display; }

private:
    QString m_xserver_path;
    QTimer m_ready_timer;
    QTcpSocket m_test_socket;
    quint16 m_display = 0;

    // QProcess may emit events during destruction, such as handling the last messages emitted by the process.
    // The last declared member will be the first to be destroyed, so QProcess has to be last here, ensuring
    // the rest of the class is still there to deal with any last-time events.
    QProcess m_process;

private slots:
    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    void processStdoutReady();
    void processStderrReady();
#ifdef Q_OS_WIN32
    void checkServerIsUp();
    void serverIsUp();
#endif
    void socketError(QAbstractSocket::SocketError error);

signals:
    void running();
    void stopped();
    void failed(const QString &error);
};



#endif // XSERVERLAUNCHER_H


