#include "pulseaudio.h"
#include <QFile>
#include <QDir>

#include <QDebug>
#include <QRegularExpression>
#include <QVersionNumber>
#include <QCoreApplication>

#include <helpers/binaryfinder.h>


PulseAudio::PulseAudio()
{

    m_qvd_pulseaudio_path = PathTools::findBin("pulseaudio", QStringList{"pulseaudio"});
    m_qvd_pulseaudio_home = PathTools::getPulseaudioHome();
    m_qvd_pulseaudio_config = PathTools::getPulseaudioBaseConfig();



    QObject::connect(&m_process, &QProcess::started, this, &PulseAudio::processStarted);
    QObject::connect(&m_process, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), this, &PulseAudio::processFinished);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, &PulseAudio::processError);
    QObject::connect(&m_process, &QProcess::readyReadStandardOutput, this, &PulseAudio::processReadyReadOutput);
    QObject::connect(&m_process, &QProcess::readyReadStandardError, this, &PulseAudio::processReadyReadError);

}

PulseAudio::~PulseAudio()
{

}

void PulseAudio::connectExisting([[maybe_unused]] const QString &pulseaudio_dir)
{
    qCritical() << "Connecting to existing PulseAudio not supported on Windows";
}

void PulseAudio::start()
{

    qInfo() << "Starting pulseaudio";
    QDir dir;
    dir.mkpath(m_qvd_pulseaudio_home);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    qInfo() << "PULSE_RUNTIME_PATH is " << m_qvd_pulseaudio_home;

    env.insert("PULSE_RUNTIME_PATH", m_qvd_pulseaudio_home);

    if (!m_qvd_pulseaudio_state_path.isEmpty()) {
        qInfo() << "PULSE_STATE_PATH is " << m_qvd_pulseaudio_state_path;
        env.insert("PULSE_STATE_PATH", m_qvd_pulseaudio_state_path);
    }

    m_process.setProcessEnvironment(env);

    QStringList args = QStringList({
                                       "-n",                           /* don't load the default config */
                                       "-v",                           /* verbose output */
                                       "-F",  m_qvd_pulseaudio_config, /* use our config file */
                                       "--use-pid-file=0",             /* allow multiple instances */
                                       "--exit-idle-time=-1",          /* never exit automatically */
                                   });
                                      // QString("--log-target=file:%1/pa.log").arg(m_qvd_pulseaudio_home)});

    qInfo() << "Starting pulseaudio at " << m_qvd_pulseaudio_path << " with args " << args;
    m_process.start(m_qvd_pulseaudio_path, args);



}
void PulseAudio::activateSocket() {
    qCritical() << "Activating the socket is supported on Windows";
}

QString PulseAudio::sendCommand(const QString &cmd) {
    qCritical() << "Sending PulseAudio commands is not supported on Windows";
    return "";
}

void PulseAudio::stop()
{
    m_process.terminate();
}

bool PulseAudio::isQvdPulseaudioInstalled()
{
    return true;
}

bool PulseAudio::isOpusSupported()
{
    return true; // Win32 must have it compiled in
}

bool PulseAudio::isRunning()
{
    return m_process.state() == QProcess::Running;
}

QString PulseAudio::getPulseModuleExtension()
{
    return ".dll";
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
    qInfo() << "Pulse process started";
}

void PulseAudio::processReadyReadOutput()
{
    auto data = QString(m_process.readAllStandardOutput());


    qInfo() << "Pulse: " << data;
}

void PulseAudio::processReadyReadError()
{
    auto data = m_process.readAllStandardError();

    qCritical() << "Pulse error: " << data;
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

QString PulseAudio::getPulseaudioStatePath() const
{
    return m_qvd_pulseaudio_state_path;
}

void PulseAudio::setPulseaudioStatePath(const QString &pulseaudio_state_path)
{
    m_qvd_pulseaudio_state_path = pulseaudio_state_path;
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
