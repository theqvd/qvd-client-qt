#include "qvdslavecommand.h"


QVDSlaveCommand::~QVDSlaveCommand()
{
    if ( m_http )
        delete m_http;
}

void QVDSlaveCommand::init(QTcpSocket *socket, const QString &slave_key)
{
    m_socket = socket;
    m_slave_key = slave_key;
    m_http = new QVDHTTP(*socket, nullptr);
}


QNetworkRequest QVDSlaveCommand::createRequest(const QUrl &url)
{
    QNetworkRequest req = QNetworkRequest(url);

    QString headerdata = "Basic " + m_slave_key;
    req.setRawHeader("Authorization", headerdata.toLocal8Bit());
    req.setRawHeader("User-Agent", "QVDClient Qt/0.01 (Linux)");
    return req;
}

QDebug operator<<(QDebug d, const QVDSlaveCommand &cmd)
{
    d << "QVDSlaveCommand";
    return d;
}
