#include "pulseaudio.h"
#include <QFile>
#include <QDir>

#include <QDebug>
#include <QRegularExpression>
#include <QVersionNumber>

#ifdef Q_OS_UNIX
#include <signal.h>
#endif


PulseAudio::PulseAudio()
{

    m_qvd_pulseaudio_path = "/usr/lib/qvd/bin/pulseaudio";
    m_qvd_pulseaudio_home = "/tmp/qvdpulse";
    m_qvd_pulseaudio_config = "/usr/lib/qvd/etc/pulse/default.pa";


    QObject::connect(&m_process, &QProcess::started, this, &PulseAudio::processStarted);
    QObject::connect(&m_process, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), this, &PulseAudio::processFinished);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, &PulseAudio::processError);
    QObject::connect(&m_process, &QProcess::readyReadStandardOutput, this, &PulseAudio::processReadyReadOutput);
    QObject::connect(&m_process, &QProcess::readyReadStandardError, this, &PulseAudio::processReadyReadError);


    QObject::connect(&m_connection_timer, &QTimer::timeout, this, &PulseAudio::connectionTimerTick);
    QObject::connect(&m_pulse_socket, &QLocalSocket::connected, this, &PulseAudio::socketConnected);
    QObject::connect(&m_pulse_socket, &QLocalSocket::disconnected, this, &PulseAudio::socketDisconnected);

    // Qt 5.15
    //QObject::connect(&m_pulse_socket, &QLocalSocket::errorOccurred, this, &PulseAudioBase::socketError);

}

PulseAudio::~PulseAudio()
{

}

void PulseAudio::connectExisting(const QString &pulseaudio_dir)
{
    qDebug("Starting connection to existing PulseAudio");
    m_using_existing_process = true;

    QDir pulse_dir(pulseaudio_dir);

    if ( pulseaudio_dir.isEmpty() ) {
        pulse_dir = QDir(qgetenv("XDG_RUNTIME_DIR"));

        if ( !pulse_dir.cd("pulse") ) {
            qCritical() << "Failed to determine pulseaudio's directory, dir was " << pulse_dir;
            emit error("Failed to find PulseAudio directory");
            return;
        }
    }

    m_qvd_pulseaudio_home = pulse_dir.path();
    qInfo() << "Found pulse home: " << m_qvd_pulseaudio_home;

    QString filename = pulse_dir.filePath("pid");
    QFile pidfile(filename);
    if (!pidfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Failed to open pidfile " << filename;
        emit error("Failed to open pidfile");
        return;
    }

    QString pid_string = pidfile.readAll().trimmed();
    bool ok = false;
    m_pid = pid_string.toInt(&ok);

    if (!ok) {
        qCritical() << "Failed to parse pid string " << pid_string;
        emit error("Failed to parse pid string");
        return;
    }


    activateSocket();


    m_connection_timer.setSingleShot(false);
    m_connection_timer.start(50);
}

void PulseAudio::start()
{

    qInfo() << "Starting pulseaudio";
    QDir dir;
    dir.mkpath(m_qvd_pulseaudio_home);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    qInfo() << "PULSE_RUNTIME_PATH is " << m_qvd_pulseaudio_home;

    env.insert("PULSE_RUNTIME_PATH", m_qvd_pulseaudio_home);
    m_process.setProcessEnvironment(env);

    QStringList args = QStringList({
                                       "-n",                           /* don't load the default config */
                                       "-v",                           /* verbose output */
                                       "-F",  m_qvd_pulseaudio_config, /* use our config file */
                                       "--exit-idle-time=-1",          /* never exit automatically */
                                   });
                                      // QString("--log-target=file:%1/pa.log").arg(m_qvd_pulseaudio_home)});

    qInfo() << "Starting pulseaudio at " << m_qvd_pulseaudio_path << " with args " << args;
    m_process.start(m_qvd_pulseaudio_path, args);



}

void PulseAudio::activateSocket()
{

    if ( ! m_using_existing_process && (m_process.state() != QProcess::Running) ) {
        qWarning() << "Not activating socket, process is in state " << m_process.state();
        return;
    }

#ifdef Q_OS_UNIX
    int pid;

    if ( m_using_existing_process ) {
        pid = m_pid;
    } else {
        pid = static_cast<pid_t>(m_process.pid());
    }

    qDebug() << "Sending SIGUSR2 to PID " << pid;
    kill( pid, SIGUSR2);
#endif
}

QString PulseAudio::sendCommand(const QString &command)
{
    if ( !m_pulse_socket.isValid() ) {
        qCritical() << "Attempted to send command " << command << " while not connected! Socket is in state " << m_pulse_socket.state();
        return "";
    }

    qInfo() << "Sending command: " << command;
    qint64 bytes = 0;

    QString cmd = command + "\n";
    bytes = m_pulse_socket.write(cmd.toUtf8());
    m_pulse_socket.flush();
    m_pulse_socket.waitForBytesWritten(100);

    qInfo() << "Wrote " << bytes << "bytes";

    QByteArray buf;
    QString strdata;

    while(m_pulse_socket.state() == QLocalSocket::ConnectedState ) {
        m_pulse_socket.waitForReadyRead(100);
        qInfo() << "Reading data";
        QByteArray data = m_pulse_socket.read(4096);

        qInfo() << "Data read: " << data << "; " << data.length() << " bytes";
        buf += data;
        strdata = QString::fromUtf8(buf);

        if ( strdata.contains(PULSEAUDIO_PROMPT) ) {
            break;
        }
    }

    strdata.chop(PULSEAUDIO_PROMPT.length());

    return strdata;
}

void PulseAudio::stop()
{

}

bool PulseAudio::isQvdPulseaudioInstalled()
{
    return false;
}

bool PulseAudio::isOpusSupported()
{
    return false;
}

bool PulseAudio::isRunning()
{
    return m_pulse_socket.isValid();
}

QString PulseAudio::getPulseModuleExtension()
{
#ifdef Q_OS_LINUX
    return ".so";
#endif
#ifdef Q_OS_DARWIN
    return ".dylib";
#endif
#ifdef Q_OS_WIN
    return ".dll";
#endif
}

QString PulseAudio::getPulseSocketPath()
{
    return QString("%1/cli").arg(m_qvd_pulseaudio_home);
}

QString PulseAudio::getPulseModulePath()
{
    return QString("");
}

void PulseAudio::processStarted()
{
    qInfo() << "Pulse process started, waiting for initialization";
}

void PulseAudio::processReadyReadOutput()
{
    auto data = QString(m_process.readAllStandardOutput());


    qInfo() << "Pulse: " << data;
}

void PulseAudio::processReadyReadError()
{
    auto data = m_process.readAllStandardError();
    m_pulse_err_buf.add(data);



    //auto lines = getLines(m_pulse_out_buffer, data);
    while(m_pulse_err_buf.hasLine()) {
        auto line = m_pulse_err_buf.getLine();

    //for( auto line : lines) {
        qInfo() << "PA: " << line;

        if ( line.contains("Daemon startup complete") ) {
            qInfo() << "PulseAudio daemon running, requesting socket";

            activateSocket();

            //m_connection_timer.setInterval(500);
            //m_connection_timer.start();
        }

        if ( line.contains("Loaded \"module-cli-protocol") || line.contains("Failed to load module \"module-cli-protocol")) {
            // Either we loaded the module, or tried to load it a second time
            qInfo() << "PulseAudio daemon ready for connection";
            qInfo() << "Connecting to socket " << getPulseSocketPath();
            m_pulse_socket.connectToServer(getPulseSocketPath());
        }
    }


    qCritical() << "Pulse error: " << data;
}

void PulseAudio::connectionTimerTick()
{
    //activateSocket();
    //if (!QFile::exists(getPulseModulePath())) {
    //    return;
    //}

    qInfo() << "Timer: attempting connection to pulseaudio socket " << getPulseSocketPath();
    m_pulse_socket.connectToServer(getPulseSocketPath());
}

void PulseAudio::socketConnected()
{
    qInfo() << "Socket connected";
    m_connection_timer.stop();

    QString greeting = sendCommand("hello");
    QRegularExpression version = QRegularExpression("PulseAudio ([.0-9A-Za-z_-]+)");
    QRegularExpressionMatch match = version.match(greeting);

    if ( match.hasMatch() ) {
        qInfo() << "Captured: " << match.captured(1);
        m_version = QVersionNumber::fromString( match.captured(1) );
    }

    emit started();
}

void PulseAudio::socketDisconnected()
{
    qWarning() << "Pulse socket disconnected!";
}

void PulseAudio::socketError(QLocalSocket::LocalSocketError err)
{
    qCritical() << "Socket error: " << err;
    emit error(QString("Failed to connect to socket, error %1").arg(err));
}

QStringList PulseAudio::getLines(QString &buffer, const QString &new_data)
{
    QStringList ret;
    int pos = 0;
    int newline_pos = 0;

    buffer += new_data;

    while( (newline_pos =  buffer.indexOf("\n", pos)) != -1 ) {
        QString line = buffer.mid(pos, newline_pos - pos);
        while( line.right(1) == "\n" || line.right(1) == "\r" ) {
            line.chop(1);
        }

        qInfo() << "Line: " << line;
        ret.append( line );

        pos = newline_pos+1;
    }

    buffer = buffer.right(buffer.length() - pos - 1);

    return ret;
}

QString PulseAudio::getQvdPulseaudioConfig() const
{
    return m_qvd_pulseaudio_config;
}

void PulseAudio::setQvdPulseaudioConfig(const QString &qvd_pulseaudio_config)
{
    m_qvd_pulseaudio_config = qvd_pulseaudio_config;
}

QString PulseAudio::getQvdPulseaudioPath() const
{
    return m_qvd_pulseaudio_path;
}

void PulseAudio::setQvdPulseaudioPath(const QString &qvd_pulseaudio_path)
{
    m_qvd_pulseaudio_path = qvd_pulseaudio_path;
}

QString PulseAudio::getPulseaudioHome() const
{
    return m_qvd_pulseaudio_home;
}

void PulseAudio::setPulseaudioHome(const QString &pulseaudio_home)
{
    m_qvd_pulseaudio_home = pulseaudio_home;
}

void PulseAudio::processError(QProcess::ProcessError err)
{
    m_connection_timer.stop();
    emit error(QString("Process error %1").arg(err));
}

void PulseAudio::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_connection_timer.stop();
    qWarning() << "PulseAudio finished with code " << exitCode << "; status " << exitStatus;
}
