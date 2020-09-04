#include "qvdfilehelpers.h"
#include <QFile>
#include <QTextStream>

QVDFileHelpers::QVDFileHelpers()
{

}

bool QVDFileHelpers::readFile(const QString &path, QString &contents)
{
    QFile input(path);
    if ( input.open(QIODevice::ReadOnly)) {
        QTextStream inStream(&input);
        contents = inStream.readAll().trimmed();
        input.close();
        return true;
    }

    return false;
}
