#include "shareusbdevice.h"
#include <QProcess>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


ShareUSBDevice::ShareUSBDevice(const USBDevice &device) : QVDSlaveCommand(nullptr)
{
    m_busid = device.fullDevPath();

}

void ShareUSBDevice::run()
{
    qInfo() << "Will run " << m_usbip_command;


    if (! QFileInfo::exists(m_usbip_command)) {
        setError(ShareUSBDevice::BinaryNotFound);
        emit commandFailed();
        return;
    }

    auto info = QFileInfo(m_usbip_command);
    if (!info.isExecutable()) {
        setError(ShareUSBDevice::BinaryNotExecutable);
        emit commandFailed();
        return;
    }

    connect( &m_usbip_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    //connect( &m_usbip_process, &QProcess::finished, this, &ShareUSBDevice::processFinished);

    //connect( &m_usbip_process, &QProcess::finished, this, &ShareUSBDevice::processFinished);
    m_usbip_process.start(m_usbip_command, QStringList() << "bind" << m_busid);
}

void ShareUSBDevice::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if ( exitCode != 0) {
        setError(ShareUSBDevice::BindFailed);
        emit commandFailed();
        return;
    }


    QUrl url("http://localhost");
    url.setPath("/usbip");

    QNetworkRequest req = createRequest(url);
    req.setRawHeader("Connection", "Upgrade");
    req.setRawHeader("Upgrade", "qvd:usbip/1.0");



    QVDNetworkReply *ret = m_http->post(req, nullptr);
    connect(ret, &QVDNetworkReply::finished, this, &ShareUSBDevice::http_finished);

}

void ShareUSBDevice::http_finished()
{
    QStringList args{"connect", m_busid, QString(static_cast<int>(m_socket->socketDescriptor()))};

    char **argv = new char*[ static_cast<unsigned long>(args.count() + 1 )];

    for(int i=0;i<args.count();i++) {
        argv[i] = strdup( args[i].toStdString().c_str() );
    }

    argv[args.count()] = nullptr;


#if defined(Q_OS_WIN)
    // not supported
#elif defined(Q_OS_MACOS)
    // not supported
#else
    m_socket->flush();
    int socket_fd = static_cast<int>(m_socket->socketDescriptor());

    pid_t pid = fork();
    if ( pid == 1 ) {
        qCritical() << "fork failed!";
        return;
    } else if ( pid == 0 ) {

        // Qt puts the socket in non-blocking mode, remove that.
        int status = fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_GETFL, 0) & ~O_NONBLOCK);

        if (status == -1){
          perror("calling fcntl");
          exit(101);
        }

        // Redirect our socket to stdin and stdout. Currently leaving stderr as it is,
        // which should result in any messages being sent to the app's console.

        close(0); // close stdin
        close(1); // close stdout
        //close(2); // close stderr

        dup( socket_fd ); // redirect stdin to socket
        dup( socket_fd ); // redirect stout to socket

        //dup( static_cast<int>(m_socket->socketDescriptor()) );

        execv( m_usbip_command.toStdString().c_str(), argv );

        perror("execv failed");
        exit(100);
    }
#endif
    m_socket->close();
    emit commandSuccessful();
}
