#ifndef QVDFILEHELPERS_H
#define QVDFILEHELPERS_H

#include <QObject>
#include <QException>

class QVDFileHelpers
{
public:
    QVDFileHelpers();

    static bool readFile(const QString &path, QString &contents);
};



#endif // QVDFILEHELPERS_H
