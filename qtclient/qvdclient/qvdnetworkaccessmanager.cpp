#include "qvdnetworkaccessmanager.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include "qvdnetworkreply.h"


QVDNetworkAccessManager::QVDNetworkAccessManager(QTcpSocket &socket, QObject *parent = nullptr) : QNetworkAccessManager(parent), m_socket(socket)
{

}


QNetworkReply* QVDNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
	qInfo() << "Operation; request " << request.url();

	return new QVDNetworkReply(this, request, op, outgoingData, m_socket);

}
