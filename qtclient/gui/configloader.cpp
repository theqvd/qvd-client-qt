#include <QSettings>

#include "configloader.h"
#include "qvdconnectionparameters.h"

ConfigLoader::ConfigLoader()
{

}

QVDConnectionParameters ConfigLoader::loadConnectionParameters()
{
    QSettings settings;
    QVDConnectionParameters params;

    qDebug() << "Loading settings";

    settings.beginGroup("Connection");
    params.setHost( settings.value("host", "").toString());
    params.setConnectionSpeed( static_cast<QVDConnectionParameters::ConnectionSpeed>(settings.value("speed").toInt() ));
    //params.setUsername(settings.value("username").toString());
    //params.setPassword(QString::fromUtf8( QByteArray::fromBase64( settings.value("password").toByteArray() )));
    params.setSharedFolders( settings.value("shared_folders").toStringList() );
    settings.endGroup();

    return params;
}
