#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <QObject>
#include <QVersionNumber>

#ifndef QVD_VERSION_MAJOR
#define QVD_VERSION_MAJOR 0
#endif

#ifndef QVD_VERSION_MINOR
#define QVD_VERSION_MINOR 0
#endif

#ifndef QVD_VERSION_REVISION
#define QVD_VERSION_REVISION 0
#endif

#ifndef QVD_BUILD
#define QVD_BUILD 0
#endif

#ifndef QVD_COMMIT_HASH
#define QVD_COMMIT_HASH ""
#endif

#ifndef QVD_VERSION_FULL
#define QVD_VERSION_FULL ""
#endif


class VersionInfo
{
public:
    VersionInfo();
    static QVersionNumber getVersion();
    static int getBuild() { return QVD_BUILD; }
    static QString getCommit();
    static QString getFullVersion();
    static bool isRunningFromSource();
};

#endif // VERSIONINFO_H
