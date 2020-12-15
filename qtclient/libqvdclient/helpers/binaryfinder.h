#ifndef BINARYFINDER_H
#define BINARYFINDER_H

#include <QObject>

class PathTools
{
public:
    PathTools();

    static QString findBin(const QString &name, const QStringList dirs = {});

};

#endif // BINARYFINDER_H
