#ifndef SHAREUSBDEVICE_H
#define SHAREUSBDEVICE_H

#include <QObject>
#include <QProcess>
#include <QTcpSocket>

#include "qvdslavecommand.h"

#include "usbip/usbdevicelist.h"
#include "usbip/usbdevice.h"
#include "socketforwarder.h"

class ShareUSBDevice : public QVDSlaveCommand
{
    Q_OBJECT

public:
    typedef enum {
        OK,
        BinaryNotFound,
        BinaryNotExecutable,
        BindFailed
    } Error;


    ShareUSBDevice(const USBDevice &dev);

    virtual void run();

    QString usbipBinary() const { return m_usbip_command; }

    void setUsbipBinary(const QString binary) { m_usbip_command = binary; }

    Error error() const { return m_error; }

    QString busId() const;
    void setBusId(const QString &busId);

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void http_finished();

private:
    void setError(const Error &error) { m_error = error; }

    QString m_busid = "";
    QString m_usbip_command = "/usr/lib/qvd/bin/qvd-client-slaveclient-usbip";
    Error m_error;

    // QProcess may emit events during destruction, such as handling the last messages emitted by the process.
    // The last declared member will be the first to be destroyed, so QProcess has to be last here, ensuring
    // the rest of the class is still there to deal with any last-time events.
    QProcess m_usbip_process;

#ifdef Q_OS_WIN
    QTcpSocket       m_usbip_socket;
    SocketForwarder *m_forwarder;

private slots:
    void socketConnected();
    void socketError(QAbstractSocket::SocketError err);

#endif
};

QDebug operator<< (QDebug d, const ShareUSBDevice &dev);

#endif // SHAREUSBDEVICE_H
