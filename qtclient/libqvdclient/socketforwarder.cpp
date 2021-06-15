#include "socketforwarder.h"
#include <QDebug>


int64_t SocketForwarder::getBytesAtoB() const
{
    return bytes_a_to_b;
}

int64_t SocketForwarder::getBytesBtoA() const
{
    return bytes_b_to_a;
}

SocketForwarder::SocketForwarder(QObject *parent, QIODevice &a, QIODevice &b)
    : QObject(parent), m_socket_a(a), m_socket_b(b)
{
    connect(&m_socket_a, SIGNAL(readyRead()), this, SLOT(socketA_readyRead()));
    connect(&m_socket_a, SIGNAL(readChannelFinished()), this, SLOT(socketA_finished()));

    connect(&m_socket_b, SIGNAL(readyRead()), this, SLOT(socketB_readyRead()));
    connect(&m_socket_b, SIGNAL(readChannelFinished()), this, SLOT(socketB_finished()));

    /*
    if ( m_socket_a.isOpen() && m_socket_a.waitForReadyRead(0) ) {
        socketA_readyRead();
    }

    if ( m_socket_b.isOpen() && m_socket_b.waitForReadyRead(0) ) {
        socketB_readyRead();
    }
*/

}

SocketForwarder::~SocketForwarder()
{

    disconnect(&m_socket_a, SIGNAL(readyRead()), this, SLOT(socketA_readyRead()));
    disconnect(&m_socket_a, SIGNAL(readChannelFinished()), this, SLOT(socketA_finished()));

    disconnect(&m_socket_b, SIGNAL(readyRead()), this, SLOT(socketB_readyRead()));
    disconnect(&m_socket_b, SIGNAL(readChannelFinished()), this, SLOT(socketB_finished()));
}

void SocketForwarder::socketA_readyRead()
{
    QByteArray data = m_socket_a.readAll();

    bytes_a_to_b += data.length();
    m_socket_b.write(data);

    emit dataTransferred(data.length(), 0);
    emit totalStatsUpdated(bytes_a_to_b, bytes_b_to_a);
}

void SocketForwarder::socketB_readyRead()
{
    QByteArray data = m_socket_b.readAll();

    bytes_b_to_a += data.length();
    m_socket_a.write(data);

    emit dataTransferred(0, data.length());
    emit totalStatsUpdated(bytes_a_to_b, bytes_b_to_a);
}

void SocketForwarder::socketA_finished()
{
    qInfo() << "Socket A closing";

    if ( m_socket_a.bytesAvailable() > 0 && m_socket_b.isOpen() ) {
        QByteArray data = m_socket_a.readAll();
        bytes_a_to_b += data.length();
        m_socket_b.write( data );

    }

    m_socket_b.close();
}

void SocketForwarder::socketB_finished()
{
    qInfo() << "Socket B closing";

    if ( m_socket_b.bytesAvailable() > 0 && m_socket_b.isOpen() ) {
        QByteArray data = m_socket_a.readAll();
        bytes_b_to_a += data.length();
        m_socket_a.write( data );
    }

    m_socket_a.close();
}
