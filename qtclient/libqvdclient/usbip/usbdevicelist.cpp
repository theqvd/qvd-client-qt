#include "usbdevicelist.h"
#include <QString>
#include <QDebug>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#include <initguid.h>
#include "helpers/binaryfinder.h"

DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
#endif


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

    auto devices = m_device_path.entryList();

    for( auto& file : devices ) {
        qInfo() << "Creating device from " << file;
        USBDevice dev = USBDevice::fromPath(m_device_path.filePath(file));
        if ( dev.isValid() && dev.deviceClass() != USBDevice::Hub ) {
            m_devices.append(dev);
        }
    }

    emit updated();
#endif
#ifdef WIN32

    QString binary_path = PathTools::findBin("usbip");
    if ( binary_path.isEmpty() ) {
        qCritical() << "Failed to find usbip binary";
        emit updated(false);
    } else {

        m_usbip_process.execute(binary_path, QStringList{"list", "-p", "-l"});
    }
#endif
#if 0
    // Left here for a later time, alternative win32 implementation
    HDEVINFO                         devInfoHandle;
    SP_DEVICE_INTERFACE_DATA         devIntData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA devIntDetailData;
    SP_DEVINFO_DATA                  devData;

    devInfoHandle = SetupDiGetClassDevs( &GUID_DEVINTERFACE_USB_DEVICE, NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT );
    if ( devInfoHandle == INVALID_HANDLE_VALUE ) {
        qCritical() << "Failed to get handle for  GUID_DEVINTERFACE_USB_DEVICE";
        return ret;
    }

    devIntData.cbSize = sizeof(devIntData);
    DWORD index = 0;

    SetupDiEnumDeviceInterfaces(devInfoHandle, NULL, &GUID_DEVINTERFACE_USB_DEVICE, index, &devIntData);
    while( GetLastError() != ERROR_NO_MORE_ITEMS ) {
        qInfo() << "Found interface " << index;

        DWORD detail_size;
        devData.cbSize = sizeof(devData);
        SetupDiGetDeviceInterfaceDetail(devInfoHandle, &devIntData, NULL, 0, &detail_size, NULL);

        devIntDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, detail_size);
        devIntDetailData->cbSize = sizeof(*devIntDetailData);

        if ( SetupDiGetDeviceInterfaceDetail(devInfoHandle, &devIntData, devIntDetailData, detail_size, &detail_size, &devData)) {
            qInfo() << "Detail: " << devIntDetailData->DevicePath;
            //devIntDetailData->
        } else {
            qCritical() << "Failed to get detail info for index " << index;
        }

        HeapFree(GetProcessHeap(), 0, devIntDetailData);

        SetupDiEnumDeviceInterfaces(devInfoHandle, NULL, &GUID_DEVINTERFACE_USB_DEVICE, ++index, &devIntData);
    }

#endif
}

QList<USBDevice> UsbDeviceList::getDevices()
{
    return m_devices;
}
