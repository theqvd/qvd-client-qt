#ifndef PORTALLOCATOR_H
#define PORTALLOCATOR_H

#include <QObject>

class PortAllocator
{
public:
    PortAllocator();
    static quint16 findAvailablePort();
    static quint16 findAvailablePort(quint16 min, quint16 max);
};

#endif // PORTALLOCATOR_H
