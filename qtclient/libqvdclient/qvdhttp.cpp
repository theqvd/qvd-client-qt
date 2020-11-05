#include "qvdhttp.h"

QVDNetworkReply* QVDHTTP::setupHooks(QVDNetworkReply *reply)
{

	return reply;
}

QVDHTTP::QVDHTTP(QTcpSocket &socket, QObject *parent) : QObject(parent), m_socket(socket)
{

}

QVDNetworkReply *QVDHTTP::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
	QVDNetworkReply *reply = new QVDNetworkReply(this, request, op, outgoingData, m_socket);

	connect(reply, SIGNAL(finished()), this, SLOT(network_reply_finished()) );

	return reply;


}

QString QVDHTTP::getUserAgent() const
{
	return m_useragent;
}

void QVDHTTP::setUserAgent(const QString &user_agent)
{
	m_useragent = user_agent;
}

QVDNetworkReply *QVDHTTP::head(const QNetworkRequest &request)
{
	return setupHooks(createRequest(QNetworkAccessManager::Operation::HeadOperation, request));
}

QVDNetworkReply *QVDHTTP::get(const QNetworkRequest &request)
{
	return setupHooks(createRequest(QNetworkAccessManager::Operation::GetOperation, request));
}

QVDNetworkReply *QVDHTTP::post(const QNetworkRequest &request, QIODevice *data)
{
	return setupHooks(createRequest(QNetworkAccessManager::Operation::PostOperation, request, data));
}

QVDNetworkReply *QVDHTTP::put(const QNetworkRequest &request, QIODevice *data)
{
	return setupHooks(createRequest(QNetworkAccessManager::Operation::PutOperation, request, data));
}

void QVDHTTP::network_reply_finished()
{
	QNetworkReply  *reply = qobject_cast<QNetworkReply*>(sender());

	if ( reply ) {
		emit finished(reply);
	}

}

