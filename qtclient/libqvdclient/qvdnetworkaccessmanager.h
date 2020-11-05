#ifndef QVDNETWORKACCESSMANAGER_H
#define QVDNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>


class QVDNetworkAccessManager : public QNetworkAccessManager
{
private:
	QTcpSocket &m_socket;

public:
	QVDNetworkAccessManager(QTcpSocket &socket, QObject *obj);

	QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
};

#endif // QVDNETWORKACCESSMANAGER_H
