#include "xserverlauncher.h"

#include <QDir>
#include <QDebug>
#include <QNetworkProxy>



XServerLauncher::XServerLauncher()
{

    m_display = 10;

    auto current_path = QDir::currentPath();
    m_search_paths.append(current_path + "\\vcxsrv\\vcxsrv.exe");
    m_search_paths.append(qgetenv("ProgramFiles") + "\\QVD Client\\vcxsrv\\vcxsrv.exe");
    m_search_paths.append(qgetenv("ProgramFiles(x86)") + "\\QVD Client\\vcxsrv\\vcxsrv.exe");

    qDebug() << "Trying to find X server...";
    for( auto path : m_search_paths ) {
        if ( QFile::exists(path ) ) {
            qDebug() << "X server found in " << path;
            m_xserver_path = path;
            break;
        }
    }

    QObject::connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(processStderrReady()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdoutReady()));

    QObject::connect(&m_ready_timer, SIGNAL(timeout()), this, SLOT(checkServerIsUp()));

    QObject::connect(&m_test_socket, SIGNAL(connected()), this, SLOT(serverIsUp()));
    QObject::connect(&m_test_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}


void XServerLauncher::start() {
    auto opts = QStringList({   QString(":%1").arg(m_display),
                                "-listen", "inet",
                                "-nolisten", "inet6",
                                "-multiwindow",
                                "-notrayicon",
                                "-nowinkill",
                                "-clipboard",
                                "+bs",
                                "-wm",
                                "-listen", "tcp",
                                "-silent-dup-error",
                                "-ac",
                                "-nomultimonitors"
                            });

    qInfo() << "Launching " << m_xserver_path << " with arguments " << opts;
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
