#include "xserverlauncher.h"

#include <QDir>
#include <QDebug>
#include <QNetworkProxy>
#include <QString>

#include "helpers/binaryfinder.h"

XServerLauncher::XServerLauncher()
{
#ifdef Q_OS_LINUX
    // On Linux we don't launch the X Server, since it has to be already running.
    // We instead call xhost to allow NX to connect. Most distributions these days allow
    // the local user to connect, but some don't.

    auto display = qgetenv("DISPLAY");
    auto parts = display.split(':');
    auto disp = QString::fromUtf8( parts[1].split('.')[0] );

    QObject::connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(processStderrReady()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdoutReady()));


    m_display = disp.toUShort();

    qDebug() << "DISPLAY is " << display << "; parsed into " << m_display;
#endif
#ifdef Q_OS_WIN32
    m_display = 10;

    m_xserver_path = PathTools::findBin("vcxsrv", QStringList{"vcxsrv.qvd", "vcxsrv"});

    QObject::connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(processStderrReady()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdoutReady()));

    QObject::connect(&m_ready_timer, SIGNAL(timeout()), this, SLOT(checkServerIsUp()));

    QObject::connect(&m_test_socket, SIGNAL(connected()), this, SLOT(serverIsUp()));
    QObject::connect(&m_test_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
#endif

}


void XServerLauncher::start() {
#ifdef Q_OS_LINUX
    QString user = qgetenv("USER");
    qDebug() << "Allowing user " << user << " to connect to X server";

    m_process.start("xhost", { QString("+SI:localuser:%1").arg(user)});
    qDebug() << "Started process " << m_process.program() << " with args " << m_process.arguments();
#endif
#ifdef Q_OS_WIN32
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
#endif
}

void XServerLauncher::stop() {
    emit stopped();
}

void XServerLauncher::processStarted()
{
#ifdef Q_OS_LINUX
    qDebug() << "xhost process started";
#endif
#ifdef Q_OS_WIN32
    qDebug() << "X server started, waiting for it to accept connections...";
    m_ready_timer.setSingleShot(false);
    m_ready_timer.start(100);
#endif
}

void XServerLauncher::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "xhost process finished with exit code" << exitCode << ", status" << exitStatus;
    emit running();
}

void XServerLauncher::processError(QProcess::ProcessError error)
{
    qDebug() << "xhost process failed with error" << error;
    emit running();
}

void XServerLauncher::processStdoutReady()
{
    QByteArray data = m_process.readAllStandardOutput();
    qInfo() << "xhost out: " << data;
}

void XServerLauncher::processStderrReady()
{
    QByteArray data = m_process.readAllStandardError();
    qInfo() << "xhost err: " << data;
}


void XServerLauncher::socketError(QAbstractSocket::SocketError __attribute__((unused)) error)
{

}

#ifdef Q_OS_WIN32
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
#endif


XServerLauncher::~XServerLauncher()
{

}
