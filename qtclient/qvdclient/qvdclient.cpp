#include "qvdclient.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <QSysInfo>
#include <QTcpServer>
#include <QJsonParseError>

#include "qvdnetworkreply.h"
#include "qvdhttp.h"
#include "backends/qvdbackend.h"



/**
 * @brief The QVDClient class
 *
 * Handles connecting to a QVD server.
 */


QVDClient::QVDClient(QObject *parent) : QObject(parent)
{

}

QTcpSocket *QVDClient::getSocket()
{
	return m_socket;
}


QVDBackend *QVDClient::getBackend() const
{
    return m_backend;
}

void QVDClient::setBackend(QVDBackend *backend)
{
    m_backend = backend;
}

QNetworkRequest QVDClient::createRequest(const QUrl &url)
{
    QNetworkRequest req = QNetworkRequest(url);

    QString concat = getParameters().username() + ":" + getParameters().password();
	QByteArray authdata = concat.toLocal8Bit().toBase64();

	QString headerdata = "Basic " + authdata;
	req.setRawHeader("Authorization", headerdata.toLocal8Bit());

	req.setRawHeader("User-Agent", "QVDClient Qt/0.01 (Linux)");
	return req;

}

QVDConnectionParameters QVDClient::getParameters() const
{
    return m_parameters;
}

void QVDClient::setParameters(const QVDConnectionParameters &parameters)
{
    m_parameters = parameters;
}

bool QVDClient::checkReply(QVDNetworkReply *reply)
{
    Q_ASSERT(reply);

    int http_code = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
    if ( http_code == 401 ) {
		emit connectionError(ConnectionError::AuthenticationError, "Incorrect username or password");
		disconnect();
		return false;
	} else if ( http_code == 404 ) {
		emit connectionError(ConnectionError::ProtocolError, "VM list command unrecognized. This may not be a QVD server.");
		disconnect();
		return false;
	} else if ( http_code == 503 ) {
		emit connectionError(ConnectionError::ServerBlocked, "Server blocked");
		disconnect();
    } else if ( http_code >= 500 && http_code < 600 ) {
		emit connectionError(ConnectionError::ProtocolError, "Server failure.");
		disconnect();
		return false;
	} else if ( http_code != 200 ) {
		emit connectionError(ConnectionError::ProtocolError, "Unexpected HTTP code " + QString::number(http_code));
		disconnect();
		return false;
	}

	return true;
}



void QVDClient::connectToQVD() {
	qInfo() << "Connecting!";

	m_socket = new QSslSocket(this);
	m_socket->setPeerVerifyMode(QSslSocket::QueryPeer);



	m_http = new QVDHTTP(*m_socket, this);

	connect(m_socket, SIGNAL(encrypted()), this, SLOT(qvd_connectionEstablished()));
	connect(m_socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(qvd_sslErrors(QList<QSslError>)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(qvd_socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(hostFound()), this, SLOT(qvd_hostFound()));
	connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(qvd_socketStateChanged(QAbstractSocket::SocketState)));

    m_socket->connectToHostEncrypted(getParameters().host(), getParameters().port());

//	if ( !m_socket->waitForEncrypted( )) {
//		qCritical() << "Failed to establish an encrypted connection to " << m_host << ":" << m_port << ": " << m_socket->errorString();
//		return false;
//	}

//	qInfo() << "Connection established!";



}

void QVDClient::requestVMList()
{
	QUrl url = QUrl("https://localhost/");
	url.setPath("/qvd/list_of_vm");

	QNetworkRequest req = createRequest(url);

	req.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));

	QVDNetworkReply *ret = m_http->get(req);
	connect(ret, SIGNAL(finished()), this, SLOT(qvd_vmListDownloaded()));
}

void QVDClient::ping()
{
	QUrl url = QUrl("https://localhost/");
	url.setPath("/qvd/ping");

	QNetworkRequest req = createRequest(url);
	QVDNetworkReply *ret = m_http->get(req);
	connect(ret, SIGNAL(finished()), this, SLOT(qvd_Pong()));
}



void QVDClient::connectToVM(int id)
{
	QUrl url = QUrl("https://localhost/qvd/connect_to_vm");
	QUrlQuery query;
    auto geometry = getParameters().geometry();

	query.addQueryItem("id"                           , QString::number(id));
    query.addQueryItem("qvd.client.keyboard"          , getParameters().keyboard());
	query.addQueryItem("qvd.client.os"                , QSysInfo::kernelType() );
    query.addQueryItem("qvd.client.nxagent.extra_args", getParameters().nxagent_extra_args());
    query.addQueryItem("qvd.client.geometry"          , QString("%1x%2").arg(geometry.width()).arg(geometry.height()));
    query.addQueryItem("qvd.client.fullscreen"        , getParameters().fullscreen() ? "1" : "0");
    query.addQueryItem("qvd.client.printing.enabled"  , getParameters().printing() ? "1" : "0");
    query.addQueryItem("qvd.client.usb.enabled"       , getParameters().usb_forwarding() ? "1" : "0");
	query.addQueryItem("qvd.client.usb.implementation", "usbip");
    query.addQueryItem("qvd.client.link", "local");
	url.setQuery(query);


	QNetworkRequest req =  createRequest(url);
	req.setRawHeader(QByteArray("Connection"), QByteArray("Upgrade"));
	req.setRawHeader(QByteArray("Upgrade"), QByteArray("QVD/1.0"));


	qInfo() << "Making request, url " << url;

	QVDNetworkReply *ret = m_http->get(req);
    connect(ret, SIGNAL(processing()), this, SLOT(qvd_vmProcessing()));
	connect(ret, SIGNAL(finished()), this, SLOT(qvd_vmConnected()));
}

void QVDClient::disconnect()
{
	if ( m_socket ) {
		m_socket->close();
		m_socket->deleteLater();
		m_socket = nullptr;
	}
}

void QVDClient::qvd_connectionEstablished() {
	qInfo() << "In connectionEstablished()";

	emit connectionEstablished();
}

void QVDClient::qvd_vmListDownloaded()
{
	QVDNetworkReply *reply = qobject_cast<QVDNetworkReply*>(sender());
	if (!reply ) {
		qCritical() << "Not a QNetworkReply!";
		return;
	}

	if (!checkReply(reply)) {
		return;
	}


	QByteArray json_data = reply->readAll();
	qInfo() << "JSON: " << json_data;


	QList<VMInfo> running_vms;


	QJsonParseError parse_error;
	QJsonDocument doc = QJsonDocument::fromJson(json_data, &parse_error);
	if ( parse_error.error != QJsonParseError::NoError ) {
		emit connectionError(ConnectionError::ProtocolError, "Failed to parse VM list: " + parse_error.errorString());
		disconnect();
		return;
	}

	QJsonArray list = doc.array();
	for (auto value : list) {
		QJsonObject obj = value.toObject();
		qInfo() << "State: " << obj["state"].toString();

		VMInfo info;
		info.blocked = obj["blocked"].toBool();
		info.id      = obj["id"].toInt();
		info.name    = obj["name"].toString();

		QString vmstate = obj["state"].toString();

        if ( vmstate == "stopped") {
			info.state   = VMState::Stopped;
		} else if ( vmstate == "starting ") {
			info.state   = VMState::Starting;
		} else if ( vmstate == "running") {
			info.state   = VMState::Running;
		}

		running_vms.append(info);
	}

	reply->deleteLater();

	emit vmListReceived(running_vms);
}

void QVDClient::qvd_vmProcessing()
{
	qInfo() << "Processing...";
}

void QVDClient::qvd_vmConnected()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	if (!reply ) {
		qCritical() << "Not a QNetworkReply!";
		return;
	}

	qInfo() << "VM connected! Status " << reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();

    m_backend->start(m_socket);
}


void QVDClient::qvd_pong()
{

}

void QVDClient::qvd_socketError(QAbstractSocket::SocketError error)
{
	emit socketError(error);
}

void QVDClient::qvd_hostFound()
{
	emit hostFound();
}

void QVDClient::qvd_socketStateChanged(QAbstractSocket::SocketState socketState)
{
	emit socketStateChanged(socketState);
}

void QVDClient::qvd_sslErrors(const QList<QSslError> &errors) {
	for(auto error : errors) {
		qInfo() << "SSL error: " << error.errorString();

	}


	emit sslErrors(errors, m_socket->peerCertificateChain());
}
