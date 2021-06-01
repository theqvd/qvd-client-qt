#include "usbdatabase.h"
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

#include "helpers/binaryfinder.h"
UsbDatabase::UsbDatabase(const QString &path)
{
    load();

}



bool UsbDatabase::load(const QString &filename)
{

    QFile file;
    if ( !filename.isEmpty() ) {
        file.setFileName(filename);
    } else {
        file.setFileName(PathTools::getUsbDatabase());
    }

    if ( file.open(QIODevice::ReadOnly)) {
        QTextStream inStream(&file);
        int cur_vendor = 0;

        m_device_names.clear();
        m_vendor_names.clear();

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

        file.close();
    } else {
        return false;
    }

    return true;
}
