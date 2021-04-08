#include "qvdnxbackend.h"
#include <QProcess>
#include <QtGlobal>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QRandomGenerator>

#include "helpers/binaryfinder.h"
#include "helpers/portallocator.h"


QVDNXBackend::QVDNXBackend(QObject *parent) : QVDBackend(parent)
{
    QObject::connect(&m_proxy_listener, &QTcpServer::newConnection, this, &QVDNXBackend::connectionAccepted);
    QObject::connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(processStderrReady()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdoutReady()));

    QObject::connect(&m_x_server_launcher, SIGNAL(running()), this, SLOT(XServerReady()));
    QObject::connect(&m_x_server_launcher, &XServerLauncher::failed, this, &QVDNXBackend::XServerFailed);
    setNxproxyBinary( PathTools::findBin("nxproxy") );

    setSlavePort( PortAllocator::findAvailablePort() );
}

QVDNXBackend::~QVDNXBackend()
{

}

QVDConnectionParameters QVDNXBackend::getParameters() const
{
    return m_parameters;
}

void QVDNXBackend::setParameters(const QVDConnectionParameters &parameters)
{
    m_parameters = parameters;
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
    m_x_server_launcher.start();
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
    stop();
    emit finished();
}

void QVDNXBackend::processError(QProcess::ProcessError error)
{
    qInfo() << "Error running NXproxy: " << error;
}

void QVDNXBackend::processStdoutReady()
{
    QByteArray data = m_process.readAllStandardOutput();
    qInfo() << "Proxy OUT: " << data;
    emit standardOutput(data);
}


static QVDBackend::NXChannel str_to_channel(const QString& str) {
    QVDBackend::NXChannel ret = QVDBackend::NXChannel::Unknown;

    QString tmp = str.toLower().trimmed();

    if ( tmp == "x11" ) { ret = QVDBackend::NXChannel::X11; }
    else if ( tmp == "auxiliary x11" ) { ret = QVDBackend::NXChannel::X11Aux; }
    else if ( tmp == "cups")           { ret = QVDBackend::NXChannel::CUPS; }
    else if ( tmp == "smb")            { ret = QVDBackend::NXChannel::SMB; }
    else if ( tmp == "http")           { ret = QVDBackend::NXChannel::HTTP; }
    else if ( tmp == "font")           { ret = QVDBackend::NXChannel::Font; }
    else if ( tmp == "file")           { ret = QVDBackend::NXChannel::File; }
    else if ( tmp == "slave")          { ret = QVDBackend::NXChannel::Slave; }

    return ret;
}


void QVDNXBackend::processStderrReady()
{
    const QString forwarding_str = "Info: Forwarding ";
    const QString listening_str  = "Info: Listening to ";

    QRegularExpression forwarding_re("Forwarding (.*?) connections to (port|display) '(.*?)'");
    QRegularExpression listening_re("Listening to (.*?) connections on port '(.*?)'");
    QRegularExpression connection_established_re("Established X server connection");
    QRegularExpression terminated_re("Session: Session terminated");

    QByteArray data = m_process.readAllStandardError();
    //qInfo() << "Proxy ERR: " << data;


    m_buffer.add(data);

    //m_buffer += data;
  //  int pos = -1;

    //while( (pos = m_buffer.indexOf('\n')) >= 0  ) {
    while(m_buffer.hasLine()) {
        QString line = m_buffer.getLine();

        //qInfo() << "Buffer is " << m_buffer.length() << "chars, line is up to " << pos;
        //QString line = m_buffer.left(pos);
        //m_buffer.remove(0, pos+1);

        // Examples:
        // Info: Forwarding multimedia connections to port 'tcp:localhost:40001'.
        // Info: Listening to slave connections on port 'tcp:localhost:63330'.

        qInfo() << "Proxy ERR Line: " << line;

        auto forwarding_match  = forwarding_re.match( line );
        auto listening_match   = listening_re.match( line );

        auto established_match = connection_established_re.match(line);
        auto terminated_match  = terminated_re.match(line);

        if ( forwarding_match.hasMatch() ) {
            auto  channel    = str_to_channel(forwarding_match.captured(1));
            auto  type       = forwarding_match.captured(2);
            auto  port_parts = forwarding_match.captured(3).split(':');

            if ( type == "port" ) {
                if ( port_parts[0] == "tcp" ) {
                    QString host = port_parts[1];
                    quint16 port = port_parts[2].toUShort();

                    qInfo() << "Backend is forwarding " << channel << " connections to TCP " << host << ":" << port;
                    emit forwardingRemoteConnectionsToTcpPort(channel, host, port );
                } else if ( port_parts[0] == "unix" ) {
                    QString socket = port_parts[1];

                    qInfo() << "Backend is forwarding " << channel << " connections to UNIX socket " << socket;
                    emit forwardingRemoteConnectionsToUnixSocket(channel, socket);
                } else {
                    qWarning() << "Failed to parse port type: '" << port_parts[0] << "'. Line was: " << line;
                }
            } else {
                QString host = port_parts[0];
                quint16 display = port_parts[1].toUShort();

                qInfo() << "Backend is forwarding " << channel << " connections to X11 display " << host << ":" << display;
                emit forwardingRemoteConnectionsToDisplay(channel, host, display);
            }


        }

        if ( listening_match.hasMatch() ) {
            auto channel    = str_to_channel(listening_match.captured(1));
            auto port_parts = listening_match.captured(2).split(':');

            if ( port_parts[0] == "tcp" ) {
                quint16 port = port_parts[2].toUShort();

                qInfo() << "Backend is forwarding " << channel << " connections to TCP port " << port;
                emit listeningOnTcpPort(channel, port );
            } else if ( port_parts[0] == "unix" ) {
                QString socket = port_parts[1];

                qInfo() << "Backend is forwarding " << channel << " connections to UNIX socket " << socket;
                emit listeningOnUnixSocket(channel, socket);
            } else {
                qWarning() << "Failed to parse port type: '" << port_parts[0] << "'. Line was: " << line;
            }
        }

        if ( established_match.hasMatch() ) {
            qInfo() << "Backend has established the connection";
            emit connectionEstablished();
        }

        if ( terminated_match.hasMatch() ) {
            qInfo() << "Connection terminated";
            emit connectionTerminated();
        }

    }



    emit standardError(data);
}

void QVDNXBackend::XServerReady()
{
    auto nxproxy_args = QStringList({"-S"});

    nxproxy_args.append(QString("slave=%1").arg(slavePort()));

    if ( parameters().printing() ) {
        nxproxy_args.append(QString("cups=%1").arg(cupsPort()));
    }

    if ( parameters().audio() ) {
        nxproxy_args.append(QString("media=%1").arg(audioPort()));
    }

    qInfo() << m_parameters.printing();

    if ( m_parameters.printing() ) {
        #ifdef Q_OS_WIN
            nxproxy_args << "smb=443";
        #else
            nxproxy_args << "cups=631";
        #endif
    }

    if ( m_parameters.fullscreen() ) {
        nxproxy_args << "fullscreen=1";
    }

    qInfo() << "Starting listener at localhost";
    m_proxy_listener.listen(QHostAddress::LocalHost, 0);

    if ( m_proxy_listener.serverPort() <= m_nx_proxy_port_offset ) {
        // On Windows Server 2003 and earlier, random listening ports generates numbers from 1025 to 5000.
        // This is extremely likely to fall below the the minimum port used by NX, which is 4000.
        // In such a case, we'll obtain an usable port by hand.
        //
        // https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind

        qWarning() << "Tried listening on random port, got port " << m_proxy_listener.serverPort() << ", below for nxproxy offset " << m_nx_proxy_port_offset << ", applying workaround";

        m_proxy_listener.close();
        auto rng = QRandomGenerator::global();

        while( !m_proxy_listener.isListening() ) {
            quint16 port = rng->bounded(m_nx_proxy_port_offset + 1, 65535);

            qDebug() << "Trying port " << port;
            bool ret = m_proxy_listener.listen(QHostAddress::LocalHost, port);
            qDebug() << "Listening " << (ret ? "successful" : "failed, will try again.");
        }
    }



    qInfo() << "Listening at port " << m_proxy_listener.serverPort();

    nxproxy_args.append(QString("localhost:%1").arg(m_proxy_listener.serverPort() - m_nx_proxy_port_offset));

    qInfo() << "Starting process " << nxproxyBinary() << " with arguments " << nxproxy_args << QString(" against display on 127.0.0.1:%1").arg(m_x_server_launcher.display());

    auto env = QProcessEnvironment::systemEnvironment();

    qInfo() << "Passing " << QCoreApplication::applicationFilePath() << " as NX_CLIENT";

    env.insert("NX_CLIENT", QCoreApplication::applicationFilePath());

#ifdef Q_OS_WIN
    env.insert("DISPLAY", QString("127.0.0.1:%1").arg( m_x_server_launcher.display() ));
    env.insert("NX_ROOT", qgetenv("LOCALAPPDATA"));
#endif

    m_process.setProcessEnvironment(env);


    //qputenv("DISPLAY", QString("127.0.0.1:%1").arg( m_x_server_launcher.display() ).toUtf8());
    qInfo() << "DISPLAY = " << env.value("DISPLAY", "[none]");
    m_process.start( nxproxyBinary(), nxproxy_args );
}

void QVDNXBackend::XServerFailed(const QString &error)
{
    qCritical() << "X server failed: " << error;
    emit failed(error);
}



