#include "xserverlauncher.h"

#include <QDir>
#include <QDebug>
#include <QNetworkProxy>
#include <QString>



XServerLauncher::XServerLauncher()
{
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

}


void XServerLauncher::start() {
    QString user = qgetenv("USER");
    qDebug() << "Allowing user " << user << " to connect to X server";

    m_process.start("xhost", { QString("+SI:localuser:%1").arg(user)});
    qDebug() << "Started process " << m_process.program() << " with args " << m_process.arguments();
}

void XServerLauncher::stop() {
    emit stopped();
}

void XServerLauncher::processStarted()
{
    qDebug() << "xhost process started";
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

void XServerLauncher::checkServerIsUp()
{

}

void XServerLauncher::serverIsUp()
{

}

void XServerLauncher::socketError(QAbstractSocket::SocketError __attribute__((unused)) error)
{

}

XServerLauncher::~XServerLauncher()
{

}
