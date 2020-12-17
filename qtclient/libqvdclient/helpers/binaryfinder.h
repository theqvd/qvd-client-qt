#ifndef BINARYFINDER_H
#define BINARYFINDER_H

#include <QObject>

class PathTools
{
public:
    PathTools();

    /**
     * @brief Returns the path to the log directory
     * @return Log directory
     */
    static QString getLogDir();

    /**
     * @brief Finds the full path to a binary
     * @param name Name of the binary, without the extension
     * @param dirs List of directories (relative) where to look for the program
     */

    static QString findBin(const QString &name, const QStringList dirs = {});

};

#endif // BINARYFINDER_H
