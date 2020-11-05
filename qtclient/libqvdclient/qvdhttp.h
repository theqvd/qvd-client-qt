#ifndef QVDHTTP_H
#define QVDHTTP_H

#include <QObject>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include "qvdnetworkreply.h"


class QVDHTTP : public QObject
{
	Q_OBJECT

private:
	QString m_useragent;
	QTcpSocket &m_socket;

	QVDNetworkReply* setupHooks(QVDNetworkReply *reply);

public:


	QVDHTTP(QTcpSocket &sock, QObject *parent = Q_NULLPTR);
	QVDNetworkReply* createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData = Q_NULLPTR);

	QString getUserAgent() const;
	void setUserAgent(const QString& agent);


	QVDNetworkReply* head(const QNetworkRequest &request);
	QVDNetworkReply* get(const QNetworkRequest &request);
	QVDNetworkReply* post(const QNetworkRequest &request, QIODevice *data);
	QVDNetworkReply* put(const QNetworkRequest &request, QIODevice *data);

signals:
	void finished(QNetworkReply *reply);

private slots:
	void network_reply_finished();


};

#endif // QVDHTTP_H
