#ifndef SHAREUSBDEVICE_H
#define SHAREUSBDEVICE_H

#include <QObject>
#include <QProcess>

#include "qvdslavecommand.h"

#include "usbip/qvdusbip.h"
#include "usbip/usbdevice.h"


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
    QProcess m_usbip_process;
};

QDebug operator<< (QDebug d, const ShareUSBDevice &dev);

#endif // SHAREUSBDEVICE_H
