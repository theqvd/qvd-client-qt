#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QObject>
#include "qvdconnectionparameters.h"

class ConfigLoader
{
public:
    ConfigLoader();

    static QVDConnectionParameters loadConnectionParameters();
};

#endif // CONFIGLOADER_H
