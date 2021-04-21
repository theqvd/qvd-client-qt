#include "binaryfinder.h"

#include <QDir>
#include <QFile>
#include <QDebug>

#include <QStringList>
#include <QList>

#include <QCoreApplication>

PathTools::PathTools()
{

}

QString PathTools::getLogDir()
{
#ifdef Q_OS_UNIX
    QString home = qgetenv("HOME");
    return home + "/.local/share/qvd-client-qt/logs";
#else
    QString appdata = qgetenv("LOCALAPPDATA");
    return appdata + "\\QVD Client Qt\\Logs";
#endif
}

QString PathTools::getPulseaudioHome()
{
#ifdef Q_OS_UNIX
    QString home = qgetenv("HOME");
    return home + "/.local/share/qvd-client-qt/pulseaudio";
#else
    QString appdata = qgetenv("LOCALAPPDATA");
    return appdata + "\\QVD Client Qt\\PulseAudio\\Home";
#endif
}

QString PathTools::getPulseaudioStateDir()
{
#ifdef Q_OS_UNIX
    QString home = qgetenv("HOME");
    return home + "/.local/share/qvd-client-qt/pulseaudio-state";
#else
    QString appdata = qgetenv("LOCALAPPDATA");
    return appdata + "\\QVD Client Qt\\PulseAudio\\State";
#endif
}

QString PathTools::getPulseaudioBaseConfig()
{
#ifdef Q_OS_WIN

    QDir app_dir(QCoreApplication::applicationDirPath());
    qDebug() << "Trying to find default.pa under " << app_dir;

    if ( app_dir.exists("pulseaudio/default.pa") ) {
        qDebug() << "default.pa found in " << app_dir;
        return QDir::toNativeSeparators( app_dir.absoluteFilePath("pulseaudio/default.pa")  );
    }

    QList<QDir> root_dirs{ QDir(qgetenv("PROGRAMFILES")), QDir(qgetenv("PROGRAMFILES(x86)")), QDir(QCoreApplication::applicationDirPath()) };
    for(auto dir : root_dirs ) {
        qDebug() << "Trying to find default.pa under " << dir;

        if ( dir.exists("QVD Client Qt/pulseaudio/default.pa") ) {
            qDebug() << "default.pa found in " << dir;
            return QDir::toNativeSeparators( dir.absoluteFilePath("QVD Client Qt/pulseaudio/default.pa"));
        }
        if ( dir.exists("QVD Client/pulseaudio/default.pa") ) {
            qDebug() << "default.pa found in " << dir;
            return QDir::toNativeSeparators( dir.absoluteFilePath("QVD Client/pulseaudio/default.pa"));
        }

    }

    qCritical() << "Failed to find default.pa";
    return "";
#else
    qCritical() << "Not implemented on this OS";
    return "";
#endif
}

QString PathTools::findBin(const QString &name, const QStringList dirs)
{
    QList<QDir> search_paths;

    qInfo() << "Searching for " << name << "in" << dirs;


    // First, search whenever our binary is
    search_paths.append(QCoreApplication::applicationDirPath());


    QDir current_dir = QDir::current();
    QDir app_dir = QDir(QCoreApplication::applicationDirPath());



#ifdef Q_OS_WIN
    QStringList dirs_copy = dirs;
    dirs_copy.append("QVD Client Qt");
    dirs_copy.append("QVD Client Qt\\pulseaudio");

    dirs_copy.append("QVD Client");
    dirs_copy.append("QVD Client\\pulseaudio");

    QList<QDir> root_dirs{ QDir(qgetenv("PROGRAMFILES")), QDir(qgetenv("PROGRAMFILES(x86)")), QDir(QCoreApplication::applicationDirPath()) };

    for( auto dir : dirs_copy ) {
        for ( auto root: root_dirs ) {
            qDebug() << "Trying " << root << "/" << dir;

            if ( root.cd(dir) ) {
                qDebug() << "Found dir " << root;
                search_paths.append(root);

                qDebug() << "Trying " << root << "/bin";
                if ( root.cd("bin")) {
                    qDebug() << "Found dir " << root;
                    search_paths.append(root);
                }

            }
        }
    }
#endif
#ifdef Q_OS_UNIX
    search_paths.append(QDir::current());
    search_paths.append(QDir("/usr/lib/qvd/bin"));
    search_paths.append(QDir("/usr/bin"));
    search_paths.append(QDir("/bin"));
    search_paths.append(QDir("/usr/libexec/openssh"));
    search_paths.append(QDir("/usr/lib/openssh"));
    search_paths.append(QDir("/usr/lib/ssh"));

    // OSX
    search_paths.append(QDir(qgetenv("HOME") + "/qvdlibs/nx/Contents/Resources/usr/lib/qvd/bin/"));
    search_paths.append(QDir("/opt/local/bin")); // OS
    search_paths.append(QDir("/usr/libexec")); // OSX
#endif

    for( QDir path : search_paths ) {
        QString file_path = path.filePath(name);

#ifdef Q_OS_WIN
        file_path.append(".exe");
#endif
        qDebug() << "Checking for " << file_path;
        if ( QFile::exists(file_path)) {
            qInfo() << "Found at " << file_path;
            return file_path;
        }
    }

    qCritical() << "Failed to find " << name << "; looked in " << search_paths;
    return "";

}
