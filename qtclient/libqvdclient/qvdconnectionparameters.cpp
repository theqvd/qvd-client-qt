#include "qvdconnectionparameters.h"
#include <QDebug>

QVDConnectionParameters::QVDConnectionParameters()
{

}

QString QVDConnectionParameters::host() const
{
    return m_host;
}

void QVDConnectionParameters::setHost(const QString &host)
{
    m_host = host;
}

quint16 QVDConnectionParameters::port() const
{
    return m_port;
}

void QVDConnectionParameters::setPort(quint16 port)
{
    m_port = port;
}

QString QVDConnectionParameters::username() const
{
    return m_username;
}

void QVDConnectionParameters::setUsername(const QString &username)
{
    m_username = username;
}

QString QVDConnectionParameters::password() const
{
    return m_password;
}

void QVDConnectionParameters::setPassword(const QString &password)
{
    m_password = password;
}

QString QVDConnectionParameters::secondFactor() const
{
    return m_second_factor;
}

void QVDConnectionParameters::setSecondFactor(const QString &second_factor)
{
    m_second_factor = second_factor;
}

QString QVDConnectionParameters::token() const
{
    return m_token;
}

void QVDConnectionParameters::setToken(const QString &token)
{
    m_token = token;
}

QVDConnectionParameters::ConnectionSpeed QVDConnectionParameters::connectionSpeed() const
{
    return m_connectionSpeed;
}

void QVDConnectionParameters::setConnectionSpeed(const ConnectionSpeed &connectionSpeed)
{
    m_connectionSpeed = connectionSpeed;
}

QSize QVDConnectionParameters::geometry() const
{
    return m_geometry;
}

void QVDConnectionParameters::setGeometry(const QSize &geometry)
{
    m_geometry = geometry;
}

QString QVDConnectionParameters::keyboard() const
{
    return m_keyboard;
}

void QVDConnectionParameters::setKeyboard(const QString &keyboard)
{
    m_keyboard = keyboard;
}

QString QVDConnectionParameters::nxagent_extra_args() const
{
    return m_nxagent_extra_args;
}

QString QVDConnectionParameters::nxproxy_extra_args() const
{
    return m_nxproxy_extra_args;
}

void QVDConnectionParameters::setNxagent_extra_args(const QString &nxagent_extra_args)
{
    m_nxagent_extra_args = nxagent_extra_args;
}

void QVDConnectionParameters::setNxproxy_extra_args(const QString &nxproxy_extra_args)
{
    m_nxproxy_extra_args = nxproxy_extra_args;
}

bool QVDConnectionParameters::fullscreen() const
{
    return m_fullscreen;
}

void QVDConnectionParameters::setFullscreen(bool fullscreen)
{
    m_fullscreen = fullscreen;
}

bool QVDConnectionParameters::printing() const
{
    return m_printing;
}

void QVDConnectionParameters::setPrinting(bool printing)
{
    m_printing = printing;
}

bool QVDConnectionParameters::usb_forwarding() const
{
    return m_usb_forwarding;
}

void QVDConnectionParameters::setUsb_forwarding(bool usb_forwarding)
{
    m_usb_forwarding = usb_forwarding;
}

QStringList QVDConnectionParameters::EnvironmentVariables() const
{
    return m_environment_variables;
}

void QVDConnectionParameters::setEnvironmentVariables(const QStringList &environment_variables)
{
    m_environment_variables = environment_variables;
}

QStringList QVDConnectionParameters::sharedFolders() const
{
    return m_shared_folders;
}

void QVDConnectionParameters::setSharedFolders(const QStringList &shared_folders)
{
    m_shared_folders = shared_folders;
}

QList<USBDevice> QVDConnectionParameters::sharedUsbDevices() const
{
    return m_shared_usb_devices;
}

void QVDConnectionParameters::setSharedUsbDevices(const QList<USBDevice> &shared_usb_devices)
{
    m_shared_usb_devices = shared_usb_devices;
}

bool QVDConnectionParameters::audio() const
{
    return m_audio;
}

void QVDConnectionParameters::setAudio(bool audio)
{
    m_audio = audio;
}

bool QVDConnectionParameters::microphone() const
{
    return m_microphone;
}

void QVDConnectionParameters::setMicrophone(bool microphone)
{
    m_microphone = microphone;
}

bool QVDConnectionParameters::audioCompression() const
{
    return m_audio_compression;
}

void QVDConnectionParameters::setAudioCompression(bool audio_compression)
{
    m_audio_compression = audio_compression;
}

QString QVDConnectionParameters::timezone() const
{
    return m_timezone;
}

void QVDConnectionParameters::setTimezone(const QString &new_timezone)
{
    m_timezone = new_timezone;
}

QDebug operator<<(QDebug debug, const QVDConnectionParameters &p)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "{"
                    << " Host = '" << p.host() << "'"
                    << "; Port = " << p.port()
                    << "; User = '" << p.username() << "'"
                    << "; Password = '" << "********" /* p.password() */ << "'"
                    << "; Second factor = " << p.secondFactor()
                    << "; Speed = " << p.connectionSpeed()
                    << "; Geometry = " << p.geometry()
                    << "; Keyboard = '" << p.keyboard() << "'"
                    << "; ExtraArgs = '" << p.nxagent_extra_args() << "'"
                    << "; NxProxyExtraArgs = '" << p.nxproxy_extra_args() << "'"
                    << "; FullScreen = " << p.fullscreen()
                    << "; Printing = " << p.printing()
                    << "; USBForwarding = " << p.usb_forwarding()
                    << "; SharedFolders = " << p.sharedFolders()
                    << "; EnvironmentVariables = " << p.EnvironmentVariables()
                    << "; Audio = " << p.audio()
                    << "; Microphone = " << p.microphone()
                    << "; Audio compression = " << p.audioCompression()
                    << "; Timezone = " << p.timezone()
                    << "}";
    return debug;
}
