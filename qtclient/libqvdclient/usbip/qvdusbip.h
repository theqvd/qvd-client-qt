#ifndef QVDUSBIP_H
#define QVDUSBIP_H

#include <QObject>
#include <QList>
#include <QDir>
#include <QMap>
#include <QPair>


#include "usbdevice.h"


class QVDUSBIP
{
public:
    static QVDUSBIP &getInstance() {
        static QVDUSBIP inst;
        return inst;
    }



    QList<USBDevice> getDevices();

    QDir getDevicePath() const;
    void setDevicePath(const QDir &device_path);

    QString getVendorName(int vendor) {
        return m_vendor_names[vendor];
    }

    QString getDeviceName(int vendor, int product) {
        return m_device_names[ (vendor << 16) + product  ];
    }

private:
    QVDUSBIP();
    QVDUSBIP(const QVDUSBIP &) = delete;
    QVDUSBIP& operator=(const QVDUSBIP &) = delete;

    QDir m_device_path{ "/sys/bus/usb/devices"};

    QMap<int,QString> m_vendor_names;
    QMap<int,QString> m_device_names;


};

#endif // QVDUSBIP_H
