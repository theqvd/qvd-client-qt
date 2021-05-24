#include "qvdusbip.h"
#include <QString>
#include <QDebug>

QVDUSBIP::QVDUSBIP()
{

    QFile input("/usr/share/hwdata/usb.ids");

    if ( input.open(QIODevice::ReadOnly)) {
        QTextStream inStream(&input);
        int cur_vendor = 0;

        while( !inStream.atEnd() ) {
            auto line = inStream.readLine();
            bool parse_ok = false;
            auto comment_pos = line.indexOf("#");
            if ( comment_pos >= 0 ) {
                line.truncate(comment_pos);
            }

            if ( line.trimmed().isEmpty() )
                continue;


            if ( line.startsWith("\t\t") ) {
               // nothing
            } else if ( line.startsWith("\t")) {

                auto id   = line.midRef(1, 4).toInt(&parse_ok, 16);

                if ( parse_ok && cur_vendor != 0 ) {
                    auto desc = line.mid(6).trimmed();
                    m_device_names.insert( (cur_vendor << 16) | id, desc );
                }
            } else {
                auto id = line.leftRef(4).toInt(&parse_ok, 16);

                if ( parse_ok ) {
                    auto desc = line.right(line.length() - 4).trimmed();
                    cur_vendor = id;
                    m_vendor_names.insert(id, desc);
                } else {
                    cur_vendor = 0;
                }
            }

        }

        input.close();

    }
}

QDir QVDUSBIP::getDevicePath() const
{
    return m_device_path;
}

void QVDUSBIP::setDevicePath(const QDir &device_path)
{
    m_device_path = device_path;
}

QList<USBDevice> QVDUSBIP::getDevices()
{
    QList<USBDevice> ret;

    qInfo() << "Retrieving devices from " << m_device_path;

    auto devices = m_device_path.entryList();

    for( auto& file : devices ) {
        qInfo() << "Creating device from " << file;
        USBDevice dev = USBDevice::fromPath(m_device_path.filePath(file));
        if ( dev.isValid() && dev.deviceClass() != USBDevice::Hub ) {
            ret.append(dev);
        }
    }

    return ret;
}
