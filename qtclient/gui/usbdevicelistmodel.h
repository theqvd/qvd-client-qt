#ifndef USBDEVICELISTMODEL_H
#define USBDEVICELISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "usbip/usbdevice.h"
#include "usbip/usbdevicelist.h"


class USBDeviceListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    void setDeviceList(UsbDeviceList *list);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<USBDevice> getSelectedDevices();

    bool selectDevice(int vendor, int product, const QString &serial);

private slots:
    void deviceListUpdated(bool success);

private:
    QList<USBDevice> m_devices;
    QList<bool> m_selected;
    UsbDeviceList *m_list;
};

#endif // USBDEVICELISTMODEL_H
