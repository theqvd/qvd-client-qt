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
#include "slaveclient/qvdslaveclient.h"

/**
 * @brief The QVDClient class
 *
 * Handles connecting to a QVD server.
 */


static QString speedToString(QVDConnectionParameters::ConnectionSpeed s) {
    switch(s) {
        case QVDConnectionParameters::ConnectionSpeed::Modem: return "modem";
        case QVDConnectionParameters::ConnectionSpeed::ISDN: return "isdn";
        case QVDConnectionParameters::ConnectionSpeed::ADSL: return "adsl";
        case QVDConnectionParameters::ConnectionSpeed::WAN: return "wan";
        case QVDConnectionParameters::ConnectionSpeed::LAN: return "lan";
        case QVDConnectionParameters::ConnectionSpeed::Local: return "local";
    }

    // Avoid build warning -- this should never be reached.
    qCritical() << "Can't convert speed " << s << " to string, new constant? Please fix me! Assuming ADSL.";
    return "adsl";
}

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

    connect(m_backend, &QVDBackend::listeningOnTcpPort, this, &QVDClient::backend_listeningOnTcp);
    connect(m_backend, &QVDBackend::connectionEstablished, this, &QVDClient::backend_connectionEstablished);
    connect(m_backend, &QVDBackend::finished, this,  &QVDClient::disconnectFromQVD );
    connect(m_backend, &QVDBackend::failed, this, &QVDClient::backend_failed);
}

QNetworkRequest QVDClient::createRequest(const QUrl &url)
{
    QNetworkRequest req = QNetworkRequest(url);


    QString concat = getParameters().username() + ":" + getParameters().password();
    QByteArray authdata = concat.toLocal8Bit().toBase64();

    QString headerdata = "Basic " + authdata;
    req.setRawHeader("Authorization", headerdata.toLocal8Bit());

    req.setRawHeader("User-Agent", "QVDClient Qt/0.01 (Linux)");

    auto env = QProcessEnvironment::systemEnvironment();
    QSettings settings("Qindel","QVD Client");
    QString sKey= "envvar";
    settings.beginGroup("Params");
    int size = settings.beginReadArray("QVDEnvVar");
    for (int p = 0; p < size; ++p) {
        settings.setArrayIndex(p);
        if ( settings.value(sKey).value<QString>().length() != 0 ){
          req.setRawHeader("Auth-"+settings.value(sKey).value<QString>().toLatin1().toHex().trimmed(), env.value(settings.value(sKey).value<QString>()).toUtf8().toBase64().trimmed());
        }
    }
    settings.endArray();
    settings.endGroup();

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
        disconnectFromQVD();
        return false;
    } else if ( http_code == 404 ) {
        emit connectionError(ConnectionError::ProtocolError, "VM list command unrecognized. This may not be a QVD server.");
        disconnectFromQVD();
        return false;
    } else if ( http_code == 503 ) {
        emit connectionError(ConnectionError::ServerBlocked, "Server blocked");
        disconnectFromQVD();
    } else if ( http_code >= 500 && http_code < 600 ) {
        emit connectionError(ConnectionError::ProtocolError, "Server failure.");
        disconnectFromQVD();
        return false;
    } else if ( http_code != 200 ) {
        emit connectionError(ConnectionError::ProtocolError, "Unexpected HTTP code " + QString::number(http_code));
        disconnectFromQVD();
        return false;
    }

    return true;
}



void QVDClient::connectToQVD() {
    qInfo() << "Connecting!";

    m_socket = new QSslSocket(this);
    m_socket->setPeerVerifyMode(QSslSocket::QueryPeer);

    m_backend->setParameters(getParameters());
    if ( m_parameters.audio() && m_parameters.audioCompression() ) {
        qInfo() << "Setting up compressed audio";

        quint16 temp_audio_port = 63731;  // TODO: generate dynamically
        m_backend->setAudioPort(temp_audio_port);
    }


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


void QVDClient::handle_printer() {
    QUrl url = QUrl("https://localhost/");
    url.setPath("/printer");

    QNetworkRequest req = createRequest(url);
    QVDNetworkReply *ret = m_http->get(req);
    connect(ret, &QVDNetworkReply::finished, this, [ret]() { ret->deleteLater(); });
}


void QVDClient::stopVM(int id)
{
    QUrl url = QUrl("https://localhost/qvd/stop_vm");
    QUrlQuery query;

    query.addQueryItem("id", QString::number(id));
    url.setQuery(query);


    QNetworkRequest req =  createRequest(url);
    req.setRawHeader(QByteArray("Connection"), QByteArray("Upgrade"));
    req.setRawHeader(QByteArray("Upgrade"), QByteArray("QVD/1.0"));


    qInfo() << "Making request, url " << url;

    QVDNetworkReply *ret = m_http->get(req);
    connect(ret, &QVDNetworkReply::processing, this, &QVDClient::qvd_vmProcessing);
    connect(ret, &QVDNetworkReply::finished, this, [this,id,ret](){
        qInfo() << "Powerdown completed";
        ret->disconnect();
        ret->deleteLater();
        emit vmPoweredDown(id);
    });

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
    query.addQueryItem("qvd.client.link"              , speedToString(getParameters().connectionSpeed()));
    query.addQueryItem("qvd.client.audio.compression.enable", getParameters().audioCompression() ? "1" : "0");

    url.setQuery(query);


    QNetworkRequest req =  createRequest(url);
    req.setRawHeader(QByteArray("Connection"), QByteArray("Upgrade"));
    req.setRawHeader(QByteArray("Upgrade"), QByteArray("QVD/1.0"));


    qInfo() << "Making request, url " << url;

    QVDNetworkReply *ret = m_http->get(req);
    connect(ret, SIGNAL(processing()), this, SLOT(qvd_vmProcessing()));
    connect(ret, SIGNAL(finished()), this, SLOT(qvd_vmConnected()));

}

void QVDClient::disconnectFromQVD()
{
    if ( m_socket ) {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    for (auto slave : m_active_slave_clients) {
        qDebug() << "Stopping slave " << slave;
      //  slave->stopCommand();
        slave->deleteLater();
    }

    m_active_slave_clients.clear();

    qInfo() << "Disconnected from QVD";
    emit connectionTerminated();
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
        disconnectFromQVD();
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

void QVDClient::qvd_vmPoweredDown()
{
    qInfo() << "VM powered down";

}

void QVDClient::qvd_vmConnected()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply ) {
        qCritical() << "Not a QNetworkReply!";
        return;
    }

    qInfo() << "VM connected! Status " << reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
    if ( reply->rawHeaderList().contains("X-QVD-Slave-Key") ) {
        QString slave_key = reply->rawHeader("X-QVD-Slave-Key");

        qInfo() << "Server provided a slave channel key: " << slave_key;
        m_slave_key = slave_key;
    }

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



QVDSlaveClient *QVDClient::createSlaveClient()
{
    auto slave = new QVDSlaveClient(m_slave_port, m_slave_key);
    connect( slave, &QVDSlaveClient::commandSuccessful, this, &QVDClient::slave_success);
    connect( slave, &QVDSlaveClient::commandFailed, this, &QVDClient::slave_failure);
    connect( slave, &QVDSlaveClient::finished, this, &QVDClient::slave_done);

    m_active_slave_clients.append(slave);

    return slave;
}


void QVDClient::backend_listeningOnTcp(QVDBackend::NXChannel channel, quint16 port)
{
    if ( channel == QVDBackend::Slave ) {
        m_slave_port = port;

        for(auto path : m_parameters.sharedFolders() ) {
            qInfo() << "Sharing folder " << path << " with VM";

            auto slave = createSlaveClient();
            slave->shareFolderWithVM(path);
        }

        if ( m_parameters.usb_forwarding() ) {
            for (auto dev : m_parameters.sharedUsbDevices() ) {
                qInfo() << "Sharing USB device " << dev;

                auto slave = createSlaveClient();
                slave->shareUsbWithVM(dev);
            }
        }

        if ( m_parameters.audio() && m_parameters.audioCompression() ) {
            qInfo() << "Starting compressed audio";
            auto slave = createSlaveClient();
          //  quint16 temp_audio_port = 63731;  // TODO: generate dynamically
      //      quint16 system_audio_port = 4731; //

            //m_backend->setAudioPort(temp_audio_port);
            slave->connectCompressedAudio(m_backend->audioPort());
        }
    }
}

void QVDClient::backend_connectionEstablished()
{
    qInfo() << "Backend established the connection";
    emit vmConnected(0);
}

void QVDClient::backend_failed(const QString &error)
{
    qCritical() << "Backend failed with error: " << error;
    disconnectFromQVD();
    emit connectionError(QVDClient::ConnectionError::XServerError, error);
}

void QVDClient::slave_success(const QVDSlaveCommand &cmd)
{
    qInfo() << "Slave command" << cmd << "completed";
}

void QVDClient::slave_failure(const QVDSlaveCommand &cmd)
{
    qInfo() << "Slave command" << cmd << "failed";
}

void QVDClient::slave_done()
{
    QVDSlaveClient *slave = qobject_cast<QVDSlaveClient*>( sender() );

    if ( slave == nullptr ) {
        qCritical() << "slave_done called not from a QVDSlaveClient!";
        return;
    }

    m_active_slave_clients.removeAll(slave);
    slave->deleteLater();
}

void QVDClient::qvd_sslErrors(const QList<QSslError> &errors) {
    for(auto error : errors) {
        qInfo() << "SSL error: " << error.errorString();
    }

    bool continueConnection = false;
    emit sslErrors(errors, m_socket->peerCertificateChain(), continueConnection);

    if ( continueConnection ) {
        qInfo() << "Continuing, SSL errors accepted by user.";
    } else {
        qInfo() << "Aborting due to SSL errors";
        disconnectFromQVD();
    }

}
