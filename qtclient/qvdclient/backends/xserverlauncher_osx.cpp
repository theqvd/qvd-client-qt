#include "xserverlauncher.h"

#include <QDir>
#include <QDebug>
#include <QNetworkProxy>
#include <QString>



XServerLauncher::XServerLauncher()
{
    // On Linux we don't launch anything, so just parse whatever the current display is
    // and return it back.

    auto display = qgetenv("DISPLAY");
    auto parts = display.split(':');
    auto disp = QString::fromUtf8( parts[1].split('.')[0] );


    m_display = disp.toUShort();

    qDebug() << "DISPLAY is " << display << "; parsed into " << m_display;

}


void XServerLauncher::start() {
    emit running();
}

void XServerLauncher::stop() {
    emit stopped();
}

void XServerLauncher::processStarted()
{

}

void XServerLauncher::processFinished(int __attribute__((unused)) exitCode, __attribute__((unused)) QProcess::ExitStatus exitStatus)
{

}

void XServerLauncher::processError(QProcess::ProcessError __attribute__((unused)) error)
{

}

void XServerLauncher::processStdoutReady()
{

}

void XServerLauncher::processStderrReady()
{

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
