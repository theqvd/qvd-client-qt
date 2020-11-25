#ifndef BINARYFINDER_H
#define BINARYFINDER_H

#include <QObject>

class BinaryFinder
{
public:
    BinaryFinder();

    static QString find(const QString &name, const QStringList dirs = {});

};

#endif // BINARYFINDER_H
