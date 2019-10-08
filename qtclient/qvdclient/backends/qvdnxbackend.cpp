#include "qvdnxbackend.h"
#include <QProcess>

QVDNXBackend::QVDNXBackend(QObject *parent) : QVDBackend(parent)
{
    QObject::connect(&m_proxy_listener, &QTcpServer::newConnection, this, &QVDNXBackend::connectionAccepted);
    QObject::connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(processStderrReady()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdoutReady()));
}

QVDNXBackend::~QVDNXBackend()
{

}

QString QVDNXBackend::nxproxyBinary() const
{
    return m_nxproxyBinary;
}

void QVDNXBackend::setNxproxyBinary(const QString &nxproxy_binary)
{
    m_nxproxyBinary = nxproxy_binary;
}

void QVDNXBackend::start(QTcpSocket *socket)
{
    m_qvd_connection_socket = socket;

    auto nxproxy_args = QStringList({"-S", "cups=631", "slave=63640", "localhost:40"});



    qInfo() << "Starting listener at localhost:4040";
    m_proxy_listener.listen(QHostAddress::LocalHost, 4040);

    qInfo() << "Starting process " << nxproxyBinary() << " with arguments " << nxproxy_args;
    m_process.start( nxproxyBinary(), nxproxy_args );

}

void QVDNXBackend::stop()
{
    qInfo() << "Stopping NXProxy";

    if ( m_process.isOpen() )
        m_process.close();

    if ( m_forwarder )
        delete m_forwarder;

    m_forwarder = nullptr;

    if ( m_proxy_listener.isListening() )
        m_proxy_listener.close();

}

int64_t QVDNXBackend::bytesRead()
{
    if ( m_forwarder )
        return m_forwarder->getBytesAtoB();

    return 0;
}

int64_t QVDNXBackend::bytesWritten()
{
    if ( m_forwarder )
        return m_forwarder->getBytesBtoA();

    return 0;
}

void QVDNXBackend::connectionAccepted()
{
    qDebug() << "NXProxy connected to listener, starting forwarder";

    QTcpSocket *connection = m_proxy_listener.nextPendingConnection();

    if (m_forwarder)
        delete m_forwarder;

    m_forwarder = new SocketForwarder(this, *connection, *m_qvd_connection_socket);

    emit started();
}

void QVDNXBackend::processStarted()
{
    qInfo() << "NXproxy started";
}

void QVDNXBackend::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qInfo() << "NXproxy exited with code " << exitCode << ", status " << exitStatus;
    emit finished();
}

void QVDNXBackend::processError(QProcess::ProcessError error)
{
    qInfo() << "Error running NXproxy: " << error;
}

void QVDNXBackend::processStdoutReady()
{
    QByteArray data = m_process.readAllStandardOutput();
    qInfo() << "Proxy: " << data;
    emit standardOutput(data);
}

void QVDNXBackend::processStderrReady()
{
    QByteArray data = m_process.readAllStandardError();
    qInfo() << "Proxy: " << data;
    emit standardError(data);
}



