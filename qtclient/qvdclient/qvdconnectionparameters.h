#ifndef QVDCONNECTIONPARAMETERS_H
#define QVDCONNECTIONPARAMETERS_H
#include <QObject>
#include <QString>
#include <QSize>
#include <QDebug>

class QVDConnectionParameters
{

public:
    typedef enum ConnectionSpeed {
        Modem,
        ISDN,
        ADSL,
        WAN,
        LAN
    } ConnectionSpeed;

    QVDConnectionParameters();

    QString host() const;
    void setHost(const QString &host);

    quint16 port() const;
    void setPort(quint16 port);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    QString token() const;
    void setToken(const QString &token);

    ConnectionSpeed connectionSpeed() const;
    void setConnectionSpeed(const ConnectionSpeed &connectionSpeed);

    QSize geometry() const;
    void setGeometry(const QSize &geometry);

    QString keyboard() const;
    void setKeyboard(const QString &keyboard);

    QString nxagent_extra_args() const;
    void setNxagent_extra_args(const QString &nxagent_extra_args);

    bool fullscreen() const;
    void setFullscreen(bool fullscreen);

    bool printing() const;
    void setPrinting(bool printing);

    bool usb_forwarding() const;
    void setUsb_forwarding(bool usb_forwarding);

    QStringList EnvironmentVariables() const;
    void setEnvironmentVariables(const QStringList &environment_variables);

    QStringList sharedFolders() const;
    void setSharedFolders(const QStringList &shared_folders);

private:
    QString m_host;
    quint16 m_port = 8443;
    QString m_username;
    QString m_password;
    QString m_token;
    ConnectionSpeed m_connectionSpeed = ConnectionSpeed::ADSL;
    QSize m_geometry = QSize(1024, 768);

    // The "empty/empty" value is a special magic one that switches things into
    // transparent mode that should work in almost all cases.
    // https://sourceforge.net/p/opennx/bugs/31

    QString m_keyboard = "empty/empty";
    QString m_nxagent_extra_args = "";
    bool m_fullscreen = false;
    bool m_printing = false;
    bool m_usb_forwarding = false;

    QStringList m_shared_folders;
    QStringList m_environment_variables;
};

QDebug operator<<(QDebug debug, const QVDConnectionParameters &p);

#endif // QVDCONNECTIONPARAMETERS_H
