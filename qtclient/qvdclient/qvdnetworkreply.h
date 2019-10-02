#ifndef QVDNETWORKREPLY_H
#define QVDNETWORKREPLY_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QTcpSocket>
#include <QObject>

class QVDNetworkReply : public QNetworkReply
{
	Q_OBJECT

public slots:
	void readyRead();

private:
	QTcpSocket &m_socket;
	QByteArray m_buffer;
	qint64 m_buffer_pos;


	enum ReadState {
		Idle,
		HTTPResponse,
		Headers,
		Body,
		SwitchedProtocol,
		Error
	};

	enum ReadState m_read_state = ReadState::Idle;

public:

	QVDNetworkReply(QObject *parent, const QNetworkRequest &req, const QNetworkAccessManager::Operation op, QIODevice *outgoingData, QTcpSocket &socket);


	void abort() override;
	void close() override;
	qint64 bytesAvailable() const override;
	qint64 readData(char *data, qint64 maxlen) override;
	bool isSequential () const override;

signals:
	void processing();

};

#endif // QVDNETWORKREPLY_H
