#ifndef USBDEVICELISTMODEL_H
#define USBDEVICELISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "usbip/qvdusbip.h"
#include "usbip/usbdevice.h"


class USBDeviceListModel : public QAbstractListModel
{
public:
    USBDeviceListModel();

    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<USBDevice> getSelectedDevices();

    bool selectDevice(int vendor, int product, const QString &serial);


private:
    QList<USBDevice> m_devices;
    QList<bool> m_selected;

};

#endif // USBDEVICELISTMODEL_H
