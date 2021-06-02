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
     * @brief Returns the directory that contains pulseaudio modules
     * @return
     */
    static QString getPulseaudioModuleDir();


    /**
     * @brief Finds the full path to a binary
     * @param name Name of the binary, without the extension
     * @param dirs List of directories (relative) where to look for the program
     */

    static QString findBin(const QString &name, const QStringList dirs = {});

    static QString getUsbDatabase();

private:

    /**
     * @brief Finds the first directory that exists and contains one of the listed filenames
     * @param paths Paths to scan
     * @param must_contain List of filenames that identify the directory as correct. This check is skipped if the list is empty.
     * @return
     */
    static QString findFirstExistingDir(const QStringList &paths, const QStringList &must_contain);

    /**
     * @brief Add a value from the environment to a list.
     *
     * Utility function.
     * @param list List to add to
     * @param environment Environment variable to add
     * @returns Whether the environment variable existed and was added to list.
     */
    static bool addFromEnv(QStringList &list, const QString &environment);
};

#endif // BINARYFINDER_H
