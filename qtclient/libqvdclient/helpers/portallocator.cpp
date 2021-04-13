#include "portallocator.h"
#include <QTcpServer>
#include <random>



PortAllocator::PortAllocator()
{

}

quint16 PortAllocator::findAvailablePort()
{
    int port = 0;
    QTcpServer tcpserver;

    qDebug() << "Finding a random port on localhost...";

    tcpserver.listen(QHostAddress::LocalHost, 0);
    port = tcpserver.serverPort();
    tcpserver.close();

    qDebug() << "Found port " << port;
    return port;
}

quint16 PortAllocator::findAvailablePort(quint16 min, quint16 max)
{
    QTcpServer tcpserver;

    std::default_random_engine rng;
    std::uniform_int_distribution<quint16> rnd_port(min, max);

    quint16 port = 0;

    while( !tcpserver.isListening() ) {
        port = rnd_port(rng);

        qDebug() << "Trying port " << port;
        bool ret = tcpserver.listen(QHostAddress::LocalHost, port);
        qDebug() << "Listening " << (ret ? "successful" : "failed, will try again.");
    }

    tcpserver.close();
    return port;
}
