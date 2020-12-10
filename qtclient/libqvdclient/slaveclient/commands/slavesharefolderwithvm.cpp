#include <QtGlobal>
#include <QFileInfo>
#include <QDir>

#include "slavesharefolderwithvm.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "helpers/binaryfinder.h"

SlaveShareFolderWithVM::~SlaveShareFolderWithVM()
{
    qWarning() << "Destroying SlaveShareFolderWithVM";
}

void SlaveShareFolderWithVM::run()
{
    QUrl url("http://localhost");
    url.setPath("/shares/" + m_folder);

    QNetworkRequest req = createRequest(url);
    req.setRawHeader("Connection", "Upgrade");
    req.setRawHeader("Upgrade", "qvd:sftp/1.0");

    m_sftp_server_binary = BinaryFinder::find("sftp-server");

    QVDNetworkReply *ret = m_http->put(req, nullptr);
    connect(ret, &QVDNetworkReply::finished, this, &SlaveShareFolderWithVM::http_finished);
}

void SlaveShareFolderWithVM::http_finished()
{
    QVDNetworkReply *reply = qobject_cast<QVDNetworkReply*>(sender());
    if (!reply ) {
        qCritical() << "Not a QNetworkReply!";
        return;
    }

    //if (!checkReply(reply)) {
    //	return;
    //}

    int http_code = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
    qInfo() << "Reply from the VM: " << http_code;

    QStringList args;
    args.append(m_sftp_server_binary);


    args.append("-e");



    qInfo() << "Will run " << m_sftp_server_binary << " with args " << args;


    if (! QFileInfo::exists(m_sftp_server_binary)) {
        setError(SlaveShareFolderWithVM::BinaryNotFound);
        emit commandFailed();
        return;
    }

    auto info = QFileInfo(m_sftp_server_binary);
    if (!info.isExecutable()) {
        setError(SlaveShareFolderWithVM::BinaryNotExecutable);
        emit commandFailed();
        return;
    }

    if ( !QDir::setCurrent(m_folder) ) {
        setError(SlaveShareFolderWithVM::DirectoryNotFound);
        emit commandFailed();
        return;
    }

    char **argv = new char*[ static_cast<unsigned long>(args.count() + 1 )];

    for(int i=0;i<args.count();i++) {
        argv[i] = qstrdup( args[i].toStdString().c_str() );
    }

    argv[args.count()] = nullptr;

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

        execv( m_sftp_server_binary.toStdString().c_str(), argv );

        perror("execv failed");
        exit(100);
    }

    m_socket->close();
    emit commandSuccessful();
}

QDebug operator<<(QDebug d, const SlaveShareFolderWithVM &folder)
{
    d << "SlaveShareFolderWithVM(folder = " << folder.folder() << ")";
    return d;
}
