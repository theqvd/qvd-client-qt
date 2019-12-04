#ifndef QVDCLIENT_H
#define QVDCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QSize>
#include <QTcpServer>
#include <QProcess>
#include <QSslCertificate>
#include "QSettings"

#include "qvdhttp.h"
#include "qvdconnectionparameters.h"
#include "socketforwarder.h"
#include "backends/qvdbackend.h"


class QVDClient : public QObject
{
	Q_OBJECT
public:
	enum VMState {
		Stopped,
		Starting,
		Running
	};

	typedef struct VMInfo {
		QString name;
		int     id;
		bool    blocked;
		VMState state;
	} VMInfo;

	typedef enum ConnectionError {
		None,
		AuthenticationError,  // bad pw
		Timeout,              // command is taking too long
		VMStartError,         // vm failed to start
		ProtocolError,        // error in the protocol -- bad JSON for example
		ServerBlocked,        // Server has been blocked by the admin
		ServerError,          // Server returns an error
		Unexpected            // unforseen error that is not specifically handled

	} ConnectionError;

private:


	QSslSocket *m_socket = Q_NULLPTR;
	QVDHTTP *m_http = Q_NULLPTR;

    QVDBackend *m_backend = nullptr;



	QNetworkRequest createRequest(const QUrl &url);

    QVDConnectionParameters m_parameters;

	bool checkReply(QVDNetworkReply *reply);
public:


    explicit QVDClient(QObject *parent = nullptr);


    QString getNXProxy() const;

    void setNXProxy(const QString &proxy);

	/**
	 * @brief Returns the socket being used for the connection
	 * @return The socket being used for the connection
	 *
	 * This function is mainly intended for retrieving socket status and error information.
	 */
	QTcpSocket *getSocket();

    QSettings settings_cert; //Declaración global

	void connectToQVD();
	void requestVMList();
	void ping();
	void connectToVM(int id);

	void disconnect();



    QVDConnectionParameters getParameters() const;
    void setParameters(const QVDConnectionParameters &parameters);

    QVDBackend *getBackend() const;
    void setBackend(QVDBackend *backend);

signals:
    void connectionEstablished();
    void connectionError(QVDClient::ConnectionError error, QString error_text);

	void vmListReceived(const QList<QVDClient::VMInfo> &vm_list);
	void socketError(QAbstractSocket::SocketError  error);
	void sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain);
	void hostFound();
	void socketStateChanged(QAbstractSocket::SocketState socketState);

private slots:
	void qvd_sslErrors(const QList<QSslError> &errors);
	void qvd_connectionEstablished();
	void qvd_vmListDownloaded();
	void qvd_vmProcessing();
	void qvd_vmConnected();
	void qvd_pong();


	void qvd_socketError(QAbstractSocket::SocketError  error);
	void qvd_hostFound();
	void qvd_socketStateChanged(QAbstractSocket::SocketState socketState);
public slots:



};

#endif // QVDCLIENT_H
