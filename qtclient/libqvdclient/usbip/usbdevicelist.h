#ifndef QVDUSBIP_H
#define QVDUSBIP_H

#include <QObject>
#include <QList>
#include <QDir>
#include <QMap>
#include <QPair>
#include <QProcess>


#include "qvdclient_global.h"
#include "usbdevice.h"


class LIBQVDCLIENT_EXPORT UsbDeviceList : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Path where devices are looked for
     * @return
     */
    QDir getDevicePath() const;

    /**
     * @brief Path where to look for devices.
     *
     * This only has an effect on Linux, and is ignored on other platforms.
     * @param device_path
     */
    void setDevicePath(const QDir &device_path);


    /**
     * @brief Refresh the list of devices
     *
     * @warning This is an asynchronous operation on some operating systems, which calls
     * a process and parses its output. Don't call getDevices() right away, but
     * handle the updated() signal instead.
     */
    void refresh();



    /**
     * @brief getDevices
     * @return Current list of devices
     */
    QList<USBDevice> getDevices();


signals:
    /**
     * @brief Device list has been updated
     * @param success Whether the list has been retrieved successfully
     */
    void updated(bool success);

private:

    QDir m_device_path{ "/sys/bus/usb/devices"};
    QList<USBDevice> m_devices;

#ifdef WIN32
    QProcess m_usbip_process;
#endif


private slots:
    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processStdoutReady();
    void processStderrReady();

};

#endif // QVDUSBIP_H
