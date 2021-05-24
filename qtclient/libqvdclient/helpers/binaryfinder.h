#ifndef BINARYFINDER_H
#define BINARYFINDER_H

#include <QObject>
#include "qvdclient_global.h"

class LIBQVDCLIENT_EXPORT PathTools
{
public:
    PathTools();

    /**
     * @brief Returns the path to the log directory
     * @return Log directory
     */
    static QString getLogDir();

    /**
     * @brief Returns the home directory for pulseaudio
     * @return Path
     */
    static QString getPulseaudioHome();

    /**
     * @brief Returns the home directory for pulseaudio
     * @return Path
     */
    static QString getPulseaudioStateDir();

    /**
     * @brief Returns the file that contains the base configuration for Pulseaudio
     * @return
     */
    static QString getPulseaudioBaseConfig();


    /**
     * @brief Finds the full path to a binary
     * @param name Name of the binary, without the extension
     * @param dirs List of directories (relative) where to look for the program
     */

    static QString findBin(const QString &name, const QStringList dirs = {});

};

#endif // BINARYFINDER_H
