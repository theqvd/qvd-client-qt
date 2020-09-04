#ifndef USBDEVICE_H
#define USBDEVICE_H

#include <QObject>
#include <QDir>
#include <QDebug>

class USBDevice
{
public:
    // from https://www.usb.org/defined-class-codes
    typedef enum {
        UseDescriptor       = 0x00,
        Audio               = 0x01,
        Communications      = 0x02,
        HID                 = 0x03,
        Physical            = 0x05,
        Image               = 0x06,
        Printer             = 0x07,
        MassStorage         = 0x08,
        Hub                 = 0x09,
        CDC_Data            = 0x0a,
        SmartCard           = 0x0b,
        ContentSecurity     = 0x0d,
        Video               = 0x0e,
        PersonalHealthcare  = 0x0f,
        AudioVideo          = 0x10,
        Billboard           = 0x11,
        USBCBridge          = 0x12,
        Diagnostic          = 0xDC,
        Wireless            = 0xE0,
        Miscellaneous       = 0xEF,
        ApplicationSpecific = 0xFE,
        VendorSpecific      = 0xFF
    } DeviceClass;

    USBDevice();

    static USBDevice fromPath(const QString &path) {
        return fromPath(QDir(path));
    }

    static USBDevice fromPath(const QDir &path);


    int vendorId() const;
    void setVendorId(int vendor_id);

    int productId() const;
    void setProductId(int product_id);


    QString manufacturer() const;
    void setManufacturer(const QString &manufacturer);


    QString serial() const;
    void setSerial(const QString &serial);



    DeviceClass deviceClass() const;
    void setDeviceClass(DeviceClass device_class);

    QString product() const;
    void setProduct(const QString &product);

    bool isValid() const {
        return m_product_id !=0 && m_vendor_id != 0;
    }

    QString getDeviceId() const;

    bool operator==(const USBDevice &other) const;

    QString busnum() const;
    void setBusnum(const QString &busnum);

    QString devpath() const;
    void setDevpath(const QString &devpath);

    QString fullDevPath() const {
        return QString("%1-%2").arg(busnum()).arg(devpath());
    }

private:

    QString     m_manufacturer;
    QString     m_product;
    int         m_vendor_id = 0;
    int         m_product_id = 0;
    DeviceClass m_device_class = UseDescriptor;
    QString     m_serial;

    QString     m_busnum;
    QString     m_devpath;

};


QDebug operator<<(QDebug debug, const USBDevice &p);

#endif // USBDEVICE_H
