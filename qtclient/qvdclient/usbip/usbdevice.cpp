#include "usbdevice.h"
#include "helpers/qvdfilehelpers.h"
#include <QDebug>
#include "qvdusbip.h"

USBDevice::USBDevice()
{

}

USBDevice USBDevice::fromPath(const QDir &path)
{
    USBDevice ret;
    QString data;

    if ( QVDFileHelpers::readFile(path.filePath("bDeviceClass"), data) ) {
        bool converted;
        ret.setDeviceClass(static_cast<DeviceClass>(data.toInt(&converted, 16)));
    }

    if ( QVDFileHelpers::readFile(path.filePath("idVendor"), data) ) {
        qInfo() << "Reading from  " << path.filePath("idVendor") << "; got " << data;
        ret.setVendorId(data.toInt(nullptr, 16));
    }

    if ( QVDFileHelpers::readFile(path.filePath("idProduct"), data) ) {
        qInfo() << "Reading from  " << path.filePath("idProduct") << "; got " << data;
        ret.setProductId(data.toInt(nullptr, 16));
    }

    if ( QVDFileHelpers::readFile(path.filePath("serial"), data) ) {
        ret.setSerial(data);
    }

    if ( QVDFileHelpers::readFile(path.filePath("product"), data) ) {
        ret.setProduct(data);
    }

    if ( QVDFileHelpers::readFile(path.filePath("manufacturer"), data) ) {
        qInfo() << "Reading from  " << path.filePath("manufacturer") << "; got " << data;
        ret.setManufacturer(data);
    }

    if ( QVDFileHelpers::readFile(path.filePath("busnum"), data) ) {
        ret.setBusnum(data);
    }

    if ( QVDFileHelpers::readFile(path.filePath("devpath"), data) ) {
        ret.setDevpath(data);
    }

    auto &usb = QVDUSBIP::getInstance();
    QString hex;

    if ( ret.manufacturer().isEmpty() ) {
        ret.setManufacturer( usb.getVendorName( ret.vendorId() )  );
    }

    if ( ret.manufacturer().isEmpty() ) {
        hex.setNum(ret.vendorId(), 16);
        ret.setManufacturer("[Manufacturer 0x" + hex.rightJustified(4,'0') + "]");
    }

    if ( ret.product().isEmpty() ) {
        ret.setProduct( usb.getDeviceName( ret.vendorId(), ret.productId()) );
    }

    if ( ret.product().isEmpty() ) {
        hex.setNum(ret.productId(), 16);
        ret.setProduct("[Product 0x" + hex.rightJustified(4, '0') + "]");
    }

    return ret;
}


QString USBDevice::manufacturer() const
{
    return m_manufacturer;
}

void USBDevice::setManufacturer(const QString &manufacturer)
{
    m_manufacturer = manufacturer;
}

int USBDevice::vendorId() const
{
    return m_vendor_id;
}

void USBDevice::setVendorId(int vendor_id)
{
    m_vendor_id = vendor_id;
}

int USBDevice::productId() const
{
    return m_product_id;
}

void USBDevice::setProductId(int product_id)
{
    m_product_id = product_id;
}

QString USBDevice::serial() const
{
    return m_serial;
}

void USBDevice::setSerial(const QString &serial)
{
    m_serial = serial;
}

USBDevice::DeviceClass USBDevice::deviceClass() const
{
    return m_device_class;
}

void USBDevice::setDeviceClass(DeviceClass device_class)
{
    m_device_class = device_class;
}

QString USBDevice::product() const
{
    return m_product;
}

void USBDevice::setProduct(const QString &product)
{
    m_product = product;
}

QString USBDevice::getDeviceId() const
{
    if (!isValid()) {
        return "";
    }

    QString ret = QString().number(vendorId(), 16 ) + ":" + QString().number(productId(), 16);

    if (!serial().isEmpty() ) {
        ret += "@" + serial();
    }

    return ret;
}

bool USBDevice::operator==(const USBDevice &other) const
{
    return vendorId() == other.vendorId() && productId() == other.productId() && serial() == other.serial();
}

QString USBDevice::busnum() const
{
    return m_busnum;
}

void USBDevice::setBusnum(const QString &busnum)
{
    m_busnum = busnum;
}

QString USBDevice::devpath() const
{
    return m_devpath;
}

void USBDevice::setDevpath(const QString &devpath)
{
    m_devpath = devpath;
}

QDebug operator<<(QDebug debug, const USBDevice &p)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "{ "
                       " devPath = " << p.fullDevPath() <<
                       ", deviceClass = " << p.deviceClass() <<
                       ", vendor = " << p.manufacturer() <<
                       ", product = " << p.product()
                    << " }";

    return debug;
}



