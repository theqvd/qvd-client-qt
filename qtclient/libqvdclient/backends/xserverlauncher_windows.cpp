#include "xserverlauncher.h"

#include <QDir>
#include <QDebug>
#include <QNetworkProxy>


#include "helpers/binaryfinder.h"


XServerLauncher::XServerLauncher()
{

    m_display = 10;

    m_xserver_path = PathTools::findBin("vcxsrv", QStringList{"vcxsrv.qvd", "vcxsrv"});

    QObject::connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(processStderrReady()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdoutReady()));

    QObject::connect(&m_ready_timer, SIGNAL(timeout()), this, SLOT(checkServerIsUp()));

    QObject::connect(&m_test_socket, SIGNAL(connected()), this, SLOT(serverIsUp()));
    QObject::connect(&m_test_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}


void XServerLauncher::start() {
    QString log_dir = PathTools::getLogDir();

    auto opts = QStringList({   QString(":%1").arg(m_display),
                                "-listen", "local",
                                "-nolisten", "inet6",
                                "-multiwindow",
                                "-notrayicon",
                                "-nowinkill",
                                "-clipboard",
                                "+bs",
                               // "-wm",
                                //"-listen", "tcp",
                                "-silent-dup-error",
                                "-ac",
                                "-nomultimonitors",
                                "-logfile", log_dir + "\\vcxsrv.log",
                            });

    QFileInfo fi(m_xserver_path);

    qInfo() << "Launching " << m_xserver_path << " with arguments " << opts << " in " << fi.path();
    if ( m_xserver_path.isEmpty() ) {
        qCritical() << "Can't start X server, no path was set!";
        emit failed("Failed to start X server, binary wasn't found");
        return;
    }

    m_process.setWorkingDirectory(fi.path());
    m_process.start(m_xserver_path, opts);

}

void XServerLauncher::stop() {

}

void XServerLauncher::processStarted()
{
    qDebug() << "X server started, waiting for it to accept connections...";
    m_ready_timer.setSingleShot(false);
    m_ready_timer.start(100);
}

void XServerLauncher::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Exited with code " << exitCode << ", status " << exitStatus;
    emit stopped();
}

void XServerLauncher::processError(QProcess::ProcessError error)
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

void XServerLauncher::socketError(QAbstractSocket::SocketError error)
{
    qDebug() << "X server not up yet: " << error;
}

XServerLauncher::~XServerLauncher()
{
    m_process.kill();
}
