#include "versioninfo.h"

VersionInfo::VersionInfo()
{

}

QVersionNumber VersionInfo::getVersion()
{
    return QVersionNumber(QVD_VERSION_MAJOR, QVD_VERSION_MINOR, QVD_VERSION_REVISION);
}

QString VersionInfo::getCommit()
{
    return QString(QVD_COMMIT_HASH);
}

QString VersionInfo::getFullVersion()
{
    return QString(QVD_VERSION_FULL);
}

bool VersionInfo::isRunningFromSource()
{
    return getVersion().majorVersion() == 0;
}
