#include "usbdevicelist.h"
#include <QString>
#include <QDebug>

#ifdef Q_OS_WIN32
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#include <initguid.h>
#include "helpers/pathtools.h"

DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
#endif


UsbDeviceList::~UsbDeviceList()
{
#ifdef Q_OS_WIN
    if ( m_usbip_process.state() == QProcess::Running ) {
        m_usbip_process.terminate();
        m_usbip_process.waitForFinished(500);
    }
#endif
}

QDir UsbDeviceList::getDevicePath() const
{
    return m_device_path;
}

void UsbDeviceList::setDevicePath(const QDir &device_path)
{
    m_device_path = device_path;
}

void UsbDeviceList::refresh() {
    m_devices.clear();

#ifdef Q_OS_LINUX
    qInfo() << "Retrieving devices from " << m_device_path;

    UsbDatabase db;
    auto devices = m_device_path.entryList();

    for( auto& file : devices ) {
        qInfo() << "Creating device from " << file;
        USBDevice dev = USBDevice::fromPath(m_device_path.filePath(file), db);
        if ( dev.isValid() && dev.deviceClass() != USBDevice::Hub ) {
            m_devices.append(dev);
        }
    }

    qInfo() << "Device list done.";
    emit updated(true);
#endif
#ifdef WIN32

    QString binary_path = PathTools::findBin("usbipd");
    if ( binary_path.isEmpty() ) {
       qCritical() << "Failed to find usbip binary";
       emit updated(false);
    } else {

       QObject::connect(&m_usbip_process, &QProcess::started, this, &UsbDeviceList::processStarted);
       QObject::connect(&m_usbip_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,  &UsbDeviceList::processFinished);
       QObject::connect(&m_usbip_process, &QProcess::readyReadStandardError, this, &UsbDeviceList::processStderrReady);
       QObject::connect(&m_usbip_process, &QProcess::readyReadStandardOutput, this, &UsbDeviceList::processStdoutReady);

       m_usbip_process.setProgram(binary_path);
       m_usbip_process.setArguments({"state"});
       m_usbip_process.start();
    }
#endif
}
#ifdef WIN32
void UsbDeviceList::processStarted()
{
	    qInfo() << "USBIP started";
}

void UsbDeviceList::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	    qInfo() << "USBIP exited with code " << exitCode << ", status " << exitStatus;
}

void UsbDeviceList::processStderrReady()
{
    if (!m_usbip_process.waitForStarted()) {
        qDebug() << "Failed to get usb devices!!";
        const QString error = m_usbip_process.readAllStandardError();

        if (!error.isEmpty()) {
            qDebug () << "Exit status: " << m_usbip_process.exitStatus() << ", Error: " << error;
        }
        return;
    }
}

void UsbDeviceList::processStdoutReady()
{
        QByteArray data = m_usbip_process.readAllStandardOutput();
        qInfo() << "USBIP OUT: " << data;

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( data, &jsonError );

        if( jsonError.error != QJsonParseError::NoError ) {
              qCritical() << "fromJson failed: " << jsonError.errorString() << Qt::endl;
        }

        if( document.isObject() ) {
            QJsonObject jsonObj = document.object();
            const auto devices = jsonObj["Devices"];

            QString windev;
            QString busid;
            QString desc;
            QString instanceid;

            m_devices.clear();
            auto deviceArray = devices.toArray();

            for (const auto& device: deviceArray) {
                const auto obj = device.toObject();
                busid      = obj.value("BusId").toString();
                desc       = obj.value("Description").toString();
                instanceid = obj.value("InstanceId").toString();

                QUuid uuid;
                if (!obj.value("PersistedGuid").isUndefined()) {
                    uuid = QUuid::fromString(obj.value("PersistedGuid").toString());
                }

                USBDevice dev;

                // An example instanceId is:
                // USB\\VID_0951&PID_1643\\001CC0EC347CFC70671C2409
                //
                // We split this into parts:
                //  USB                        -- Discard
                //  VID_0951&PID_1643          -- USB Vendor and Product IDs
                //  001CC0EC347CFC70671C2409   -- Serial. May not be the actual serial number

                QStringList parts = instanceid.split("\\");
                parts.removeFirst(); // Remove the "USB" part, useless
                QString vidpid = parts.takeFirst();
                QString serial = parts.takeFirst();

                QRegularExpression reIds("VID_(\\d+)&PID_(\\d+)");
                auto match = reIds.match(vidpid);
                if ( match.hasMatch() ) {
                    dev.setVendorId( match.captured(1).toInt() );
                    dev.setProductId( match.captured(2).toInt() );
                } else {
                    qWarning() << "Failed to parse VID/PID part: " << vidpid;
                }

                dev.setBusnum(busid);
                dev.setProduct(desc);
                dev.setInstanceId(instanceid);
                dev.setSerial(serial);

                m_devices.append(dev);
            }
        }

        qInfo() << "Device list done.";
        emit updated(true);
}

#endif

QList<USBDevice> UsbDeviceList::getDevices()
{
    return m_devices;
}
