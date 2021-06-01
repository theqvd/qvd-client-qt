#include "usbdevicelistmodel.h"


void USBDeviceListModel::setDeviceList(UsbDeviceList *list)
{

}

int USBDeviceListModel::rowCount(const QModelIndex &parent [[maybe_unused]]) const
{
    return m_devices.length();
}

int USBDeviceListModel::columnCount(const QModelIndex &parent [[maybe_unused]]) const
{
    return 3;
}

QVariant USBDeviceListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ( role == Qt::CheckStateRole && index.column() == 0 ) {
        return m_selected [ index.row() ] ? Qt::Checked : Qt::Unchecked;
    }

    if ( role == Qt::DisplayRole ) {
        if ( index.column() == 0 ) {
            return m_devices[ index.row() ].manufacturer();
        } else if ( index.column() == 1 ) {
            return m_devices[ index.row() ].product();
        } else if ( index.column() == 2 ) {
            return m_devices[ index.row() ].serial();
        }
    }

    return QVariant();

}

QVariant USBDeviceListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( role != Qt::DisplayRole ) {
        return QVariant();
    }

    if ( orientation == Qt::Horizontal ) {
        switch(section) {
        case 0: return QVariant("Manufacturer");
        case 1: return QVariant("Product");
        case 2: return QVariant("Serial");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

bool USBDeviceListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if ( role == Qt::CheckStateRole && index.column() == 0) {
        m_selected[ index.row() ] = ( value == Qt::Checked );
        return true;
    }

    return false;
}

Qt::ItemFlags USBDeviceListModel::flags(const QModelIndex &index [[maybe_unused]]) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

QList<USBDevice> USBDeviceListModel::getSelectedDevices()
{
    QList<USBDevice> ret;

    for(int i=0;i<m_devices.count();i++) {
        if ( m_selected[i] ) {
            ret.append(m_devices[i]);
        }
    }

    return ret;
}

bool USBDeviceListModel::selectDevice(int vendor, int product, const QString &serial)
{
    bool found = false;

    for(int i=0;i<m_devices.count();i++) {
        auto d = m_devices[i];

        if ( d.vendorId() == vendor && d.productId() == product && (serial.isEmpty() || d.serial() == serial) ) {
            m_selected[i] = true;
            found = true;
        }
    }

    //emit dataChanged();

    return found;
}

void USBDeviceListModel::deviceListUpdated(bool success)
{
    if (!success)
        return;

    m_devices = m_list->getDevices();

    for([[maybe_unused]] auto &d : m_devices ) {
        m_selected.append(false);
    }
}


