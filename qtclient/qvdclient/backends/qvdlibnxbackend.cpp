#include "qvdlibnxbackend.h"
#include <QProcess>
#include <QtGlobal>
#include <NX.h>
#include <limits.h>
#include <QDebug>
#include <QCoreApplication>

QVDLibNXBackend::QVDLibNXBackend(QObject *parent) : QVDBackend(parent)
{
    //QObject::connect(&m_proxy_listener, &QTcpServer::newConnection, this, &QVDLibNXBackend::connectionAccepted);
    QObject::connect(&m_x_server_launcher        , SIGNAL(running())      , this, SLOT(XServerReady()));
    QObject::connect(&m_nx_timer                 , SIGNAL(timeout())      , this, SLOT(RunNX()));
    QObject::connect(&m_unencrypted_socket_server, SIGNAL(newConnection()), this, SLOT(SocketServerReady()));

}

QVDLibNXBackend::~QVDLibNXBackend()
{

}

void QVDLibNXBackend::start(QTcpSocket *socket)
{
    qInfo() << "Starting connection, NX library version " <<
               NXVersion() << "." <<
               NXMajorVersion() << "." <<
               NXMinorVersion() << "." <<
               NXPatchVersion() << "." <<
               NXMaintenancePatchVersion();

    m_qvd_connection_socket = socket;
    m_x_server_launcher.start();
}

void QVDLibNXBackend::stop()
{
    qInfo() << "Stopping NXProxy";

    if ( m_forwarder )
        delete m_forwarder;

    m_forwarder = nullptr;

    if ( m_proxy_listener.isListening() )
        m_proxy_listener.close();

}

int64_t QVDLibNXBackend::bytesRead()
{
    return 0;
}

int64_t QVDLibNXBackend::bytesWritten()
{
    return 0;
}




void QVDLibNXBackend::XServerReady()
{

    QString socket_name = QString("qvdclient.%1").arg( QCoreApplication::applicationPid());

    qDebug()  << "Starting unencrypted listener on address '" << socket_name << "'";
    m_unencrypted_socket_server.listen(socket_name);

    qDebug() << "Connecting to unencrypted listener...";
    m_unencrypted_socket_client.connectToServer(socket_name, QIODevice::ReadWrite);




//    m_process.setProcessEnvironment(env);


    //qputenv("DISPLAY", QString("127.0.0.1:%1").arg( m_x_server_launcher.display() ).toUtf8());
    //    m_process.start( nxproxyBinary(), nxproxy_args );
}

void QVDLibNXBackend::SocketServerReady()
{
    qDebug() << "Socket server accepted connection, starting forwarder";
    auto socket = m_unencrypted_socket_server.nextPendingConnection();
    m_forwarder = new SocketForwarder(this, *m_qvd_connection_socket, *socket);

    startNX();
}

void QVDLibNXBackend::startNX()
{
    //auto nxproxy_args = QStringList({"-S", "cups=631", "slave=63640", "localhost:40"});

    qInfo() << "Starting NX";

    auto sd = m_unencrypted_socket_client.socketDescriptor();
    if ( sd < 0 ) {
        qCritical() << "Failed to get file descriptor from socket, got " << sd;
        return;
    } else if ( sd > INT_MAX ) {
        qCritical() << "File descriptor " << sd << " out of range for a filehandle";
        return;
    }

    int ifd = static_cast<int>(sd);
    QString options("nx/nx,link=adsl,client=linux,cups=631:100");


    qInfo() << QString("NXTransCreate(%1, NX_MODE_SERVER, %2)").arg(ifd).arg(options);

    auto ret = NXTransCreate(ifd, NX_MODE_SERVER, options.toUtf8().data());
    if ( ret != 1) {
        qCritical() << "NXTransCreate failed with code " << ret;
        NXTransCleanupForReconnect();
        return;
    }

    m_nx_timer.setSingleShot(false);
    m_nx_timer.setInterval(0);
    m_nx_timer.start();

}

void QVDLibNXBackend::RunNX()
{
    if ( NXTransRunning(NX_FD_ANY)) {
        qInfo() << "NXTransContinue";
        // FIXME: Takes a long time.d
        struct timeval tv = {0, 1000};
        auto ret = NXTransContinue(&tv);
        qInfo() << "NXTransContinue = " << ret;
    }
}
