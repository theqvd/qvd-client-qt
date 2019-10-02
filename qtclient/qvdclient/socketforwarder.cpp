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

SocketForwarder::SocketForwarder(QObject *parent, QTcpSocket &a, QTcpSocket &b)
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

void SocketForwarder::socketA_readyRead()
{
	QByteArray data = m_socket_a.readAll();
//	qInfo() << "Forwarding " << data.length() << " bytes from A to B";
	bytes_a_to_b += data.length();
	m_socket_b.write(data);

}

void SocketForwarder::socketB_readyRead()
{
	QByteArray data = m_socket_b.readAll();
//	qInfo() << "Forwarding " << data.length() << " bytes from B to A";
	bytes_b_to_a += data.length();
	m_socket_a.write(data);

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
