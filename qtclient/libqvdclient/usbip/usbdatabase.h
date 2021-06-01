#ifndef USBDATABASE_H
#define USBDATABASE_H

#include <QObject>
#include <QFile>
#include <QMap>
#include "qvdclient_global.h"

class LIBQVDCLIENT_EXPORT UsbDatabase
{
public:
    UsbDatabase(const QString& path="");

    bool load(const QString &path = "");

    QString getVendorName(int vendor) const {
        return m_vendor_names[vendor];
    };

    QString getDeviceName(int vendor, int product) const {
        return m_device_names[ (vendor << 16) + product  ];
    };

    int getVendorCount() const { return m_vendor_names.size(); };

    int getItemCount() const { return m_device_names.size(); };


private:
    QMap<int,QString> m_vendor_names;
    QMap<int,QString> m_device_names;
};

#endif // USBDATABASE_H
