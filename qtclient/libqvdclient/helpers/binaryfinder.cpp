#include "binaryfinder.h"

#include <QDir>
#include <QFile>
#include <QDebug>

#include <QStringList>
#include <QList>

#include <QCoreApplication>

BinaryFinder::BinaryFinder()
{

}

QString BinaryFinder::find(const QString &name, const QStringList dirs)
{
    QList<QDir> search_paths;

    qInfo() << "Searching for " << name << "in" << dirs;


    //search_paths.append(QDir::current());

    // First, search whenever our binary is
    search_paths.append(QCoreApplication::applicationDirPath());


    QDir current_dir = QDir::current();
    QDir app_dir = QDir(QCoreApplication::applicationDirPath());



#ifdef Q_OS_WIN
    QStringList dirs_copy = dirs;
    dirs_copy.append("QVD Client Qt");
    dirs_copy.append("QVD Client");

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

        /*
        QDir pf(qgetenv("PROGRAMFILES"));
        QDir pfx86(qgetenv("PROGRAMFILES(x86)"));
        QDir appdir = QCoreApplication::applicationDirPath();

        if ( appdir.cd(dir) ) {
            search_paths.append(appdir);

            if ( appdir.cd("bin")) {
                search_paths.append(appdir);
            }
        }

        if ( pf.cd(dir) )  {
            search_paths.append(pf);

            if ( appdir.cd("bin")) {
                search_paths.append(appdir);
            }
        }

        if ( pfx86.cd(dir) )  {
            search_paths.append(pfx86);
        }
*/

    }
#endif
#ifdef Q_OS_UNIX
    search_paths.append(QDir("/usr/lib/qvd/bin"));
    search_paths.append(QDir("/usr/bin"));
    search_paths.append(QDir("/bin"));
#endif

    for( QDir path : search_paths ) {
        QString file_path = path.filePath(name);

#ifdef Q_OS_WIN
        file_path.append(".exe");
#endif
        qDebug() << "Checking for " << file_path;
        if ( QFile::exists(file_path)) {
            return file_path;
        }
    }

    qCritical() << "Failed to find " << name << "; looked in " << search_paths;
    return "";

}
