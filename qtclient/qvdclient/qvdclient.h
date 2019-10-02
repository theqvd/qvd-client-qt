#ifndef QVDCLIENT_H
#define QVDCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QSize>
#include <QTcpServer>
#include <QProcess>
#include <QSslCertificate>

#include "qvdhttp.h"
#include "socketforwarder.h"


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

	typedef enum ConnectionSpeed {
		Modem,
		ISDN,
		ADSL,
		WAN,
		LAN
	} ConnectionSpeed;

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
	QString m_host;
	int     m_port;
	QString m_username;
	QString m_password;

	QSslSocket *m_socket = Q_NULLPTR;
	QVDHTTP *m_http = Q_NULLPTR;

	QSize m_geometry = QSize(1024, 768);
	ConnectionSpeed m_connectionSpeed = ConnectionSpeed::ADSL;
	QString m_keyboard;
	QString m_nxagent_extra_args;
	bool m_fullscreen = false;
	bool m_printing = false;
	bool m_usb_forwarding = false;

	QNetworkRequest createRequest(const QUrl &url);

	QTcpServer m_proxy_listener;
	SocketForwarder *m_socket_forwarder;
	QProcess *m_proxy_process;
    QString m_nx_proxy;

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

	/**
	 * @brief Returns the QVD user name
	 * @return QVD user name
	 */
	QString getUsername() const;

	/**
	 * @brief Sets the QVD user name
	 * @param username
	 */
	void setUsername(const QString &username);

	QString getPassword() const;
	void setPassword(const QString &password);

	QString getHost() const;
	void setHost(const QString &host);

	int getPort() const;
	void setPort(int port);


	void connectToQVD();
	void requestVMList();
	void ping();
	void connectToVM(int id);

	void disconnect();


	ConnectionSpeed getConnectionSpeed() const;
	void setConnectionSpeed(const ConnectionSpeed &connectionSpeed);

	bool getFullscreen() const;
	void setFullscreen(bool fullscreen);

	bool getPrinting() const;
	void setPrinting(bool printing);

	bool getUsbForwarding() const;
	void setUsbForwarding(bool usb_forwarding);

	QSize getGeometry() const;
	void setGeometry(const QSize &geometry);

	QString getKeyboard() const;
	void setKeyboard(const QString &keyboard);

	QString getNxagentExtraArgs() const;
	void setNxagentExtraArgs(const QString &nxagent_extra_args);

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
	void qvd_proxyConnectionAccepted();
	void qvd_proxyOutput();
	void qvd_proxyError();
    void qvd_proxyErrorOccured(QProcess::ProcessError error);
    void qvd_proxyFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void qvd_pong();


	void qvd_socketError(QAbstractSocket::SocketError  error);
	void qvd_hostFound();
	void qvd_socketStateChanged(QAbstractSocket::SocketState socketState);
public slots:



};

#endif // QVDCLIENT_H
