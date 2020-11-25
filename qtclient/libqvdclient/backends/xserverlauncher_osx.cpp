#include "xserverlauncher.h"

#include <QDir>
#include <QDebug>
#include <QNetworkProxy>
#include <QString>



XServerLauncher::XServerLauncher()
{
    // On MacOS we need to launch XQuartz as X11 default server.

    qputenv("DISPLAY","127.0.0.1:0.0");
    auto display = qgetenv("DISPLAY");
    auto parts = display.split(':');
    auto disp = QString::fromUtf8( parts[1].split('.')[0] );

    auto XQuartzPath = "/Applications/Utilities/XQuartz.app";

    QDir XQuartz(XQuartzPath);
    if (XQuartz.exists())
    {
        m_xserver_path = XQuartzPath;
        qDebug() << "XQuartz is installed in " << XQuartzPath;
    }
    else
    {
        qDebug() << "XQuartz is not installed, QVD client needs XQuartz as X11 Server.";
    }

    m_display = disp.toUShort();

    qDebug() << "DISPLAY is " << display << "; parsed into " << m_display;

    QObject::connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(processStderrReady()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdoutReady()));

    QObject::connect(&m_ready_timer, SIGNAL(timeout()), this, SLOT(checkServerIsUp()));

    QObject::connect(&m_test_socket, SIGNAL(connected()), this, SLOT(serverIsUp()));
    QObject::connect(&m_test_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

}


void XServerLauncher::start() {
    QString XQuartzLauncher = "/opt/X11/libexec/launchd_startx";
    auto opts = QStringList({   "/opt/X11/bin/startx",
                                "--",
                                "/opt/X11/bin/Xquartz",
                                QString(":%1").arg(m_display),
                                "-iglx",
                                "+bs",
                                "-wm",
                                "-listen", "tcp",
                                "-ac"
                            });

    qInfo() << "Launching " << m_xserver_path << " with arguments " << opts;
    m_process.start(XQuartzLauncher, opts);
}

void XServerLauncher::stop() {
    emit stopped();
}

void XServerLauncher::processStarted()
{
    qDebug() << "X server started, waiting for it to accept connections...";
    m_ready_timer.setSingleShot(false);
    m_ready_timer.start(100);
}

void XServerLauncher::processFinished(int __attribute__((unused)) exitCode, __attribute__((unused)) QProcess::ExitStatus exitStatus)
{
    qDebug() << "Exited with code " << exitCode << ", status " << exitStatus;
    emit stopped();
}

void XServerLauncher::processError(QProcess::ProcessError __attribute__((unused)) error)
{
    qDebug() << "Failed with error " << error;
    emit stopped();
}

void XServerLauncher::processStdoutReady()
{
    QByteArray data = m_process.readAllStandardOutput();
    qInfo() << "X Server: " << data;
}

void XServerLauncher::processStderrReady()
{
    QByteArray data = m_process.readAllStandardError();
    qInfo() << "X Server: " << data;
}

void XServerLauncher::checkServerIsUp()
{
    qDebug() << QString("Trying to connect to the X server on port %1").arg(6000+m_display);
    m_test_socket.setProxy(QNetworkProxy::NoProxy);
    m_test_socket.connectToHost("127.0.0.1", 6000 + m_display);
}

void XServerLauncher::serverIsUp()
{
    qInfo() << "X server is up!";
    m_ready_timer.stop();
    m_test_socket.close();
    emit running();
}

void XServerLauncher::socketError(QAbstractSocket::SocketError __attribute__((unused)) error)
{
    qDebug() << "X server not up yet: " << error;
}

XServerLauncher::~XServerLauncher()
{
    m_process.kill();
}
