#ifndef USBIP_H
#define USBIP_H

#include "qvdclient_global.h"
#include "usbip/usbdevice.h"
#include <QObject>
#include <QProcess>

class LIBQVDCLIENT_EXPORT UsbIp : public QObject
{
    Q_OBJECT

public:
    UsbIp();
    ~UsbIp();

    bool bindDevice(const USBDevice &dev);
    bool bindDeviceByBusID(QString id);
    bool bindDeviceByHardwareID(QString id);

    bool unbindDevice(const USBDevice &dev);
    bool unbindDeviceByBusID(QString id);
    bool unbindDeviceByHardwareID(QString id);

    quint16 getPort() const;

private slots:
    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processStdoutReady();
    void processStderrReady();

private:
    QProcess* createProcess();
    int runElevated(const QStringList &args);


    QString m_usbip_binary;
};

#endif // USBIP_H
