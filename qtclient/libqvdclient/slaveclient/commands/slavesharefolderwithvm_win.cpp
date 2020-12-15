#include <QtGlobal>
#include <QFileInfo>
#include <QDir>
#include <QUuid>

#include "slavesharefolderwithvm.h"

#include <sys/types.h>

#include <WinSock2.h>
#include <windows.h>
//#include <WinSock2.h>

#include <QTemporaryFile>

#include "helpers/binaryfinder.h"

#pragma comment(lib, "Ws2_32.lib")

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

    m_sftp_server_binary = PathTools::findBin("win-sftp-server");


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

    QString log_path = qgetenv("LOCALAPPDATA") + "\\Qindel\\QVD Client Qt\\Logs";
    QDir dir = QDir::root();
    dir.mkpath(log_path);




    int http_code = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
    qInfo() << "Reply from the VM: " << http_code;

    QStringList args;
    QString command_line;

    // Qt is annoying and won't release temporary files even when closed:
    // QTBUG-10856
    // Hack around it.
    QUuid random_id = QUuid::createUuid();
    QString random_filename = qgetenv("TEMP") + "\\qvdsocket-" + random_id.toString(QUuid::WithoutBraces);
    QFile socket_file(random_filename);

    socket_file.open(QIODevice::ReadWrite);


    args.append(m_sftp_server_binary);

    QString log_file = log_path + "\\sftp.log";

    if ( enableDebug() ) {
        // verbose logging
        args.append("-v");
    } else {
        // remove socket file
        args.append("-R");
    }

    args.append("-d");
    args.append(m_folder);

    args.append("-L");
    args.append(log_file);

    args.append("-F");
    args.append(random_filename);

    for( auto arg : args ) {
        QString tmp = arg;
        tmp = tmp.replace("\"", "\\\"");
        tmp = tmp.replace("\\", "\\\\");

        if ( tmp.contains(" ") || tmp.contains("\t")) {
            command_line += "\"" + tmp + "\" ";
        } else {
            command_line += tmp + " ";
        }
    }





    qInfo() << "Will run " << m_sftp_server_binary << " with args " << args;
    qInfo() << "Escaped: " << command_line;

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

    PROCESS_INFORMATION processInfo;
    STARTUPINFO         startupInfo;

    startupInfo.lpReserved = NULL;
    startupInfo.cb = sizeof(STARTUPINFO);
    startupInfo.lpDesktop = NULL;
    startupInfo.lpTitle = NULL;
    startupInfo.dwFlags = 0;
    startupInfo.cbReserved2 = 0;
    startupInfo.lpReserved2 = NULL;

    wchar_t *w_cmdline = new wchar_t[command_line.length() + 1];
    wchar_t *w_directory = new wchar_t[m_folder.length() + 1];

    std::memset(w_cmdline, 0, sizeof(*w_cmdline));
    std::memset(w_directory, 0, sizeof(*w_directory));

    command_line.toWCharArray(w_cmdline);
    m_folder.toWCharArray(w_directory);

    w_cmdline[command_line.length()] = 0;
    w_directory[m_folder.length()] = 0;

    BOOL create_ret =
            CreateProcessW(
                   NULL, w_cmdline,
                   NULL, // process handle not inheritable
                   NULL, // thread handle not inheritable
                   true, // inherit handles
                   NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW | CREATE_SUSPENDED, // creation flags
                   NULL, // use parent's environment
                   w_directory, // startup directory
                   &startupInfo,
                   &processInfo);

    if (!create_ret) {
        qCritical() << "CreateProcess failed: Error " << GetLastError();
        emit commandFailed();
        return;
    }

    SOCKET socket_handle = static_cast<SOCKET>(m_socket->socketDescriptor());

    // Qt makes the socket a non-blocking one, make it block again.
    u_long mode = 0;
    int sock_ret = ioctlsocket(socket_handle, FIONBIO, &mode);
    if ( sock_ret != NO_ERROR ) {
        qCritical() << "ioctlsocket failed: " << GetLastError();
        emit commandFailed();
        return;
    }


    WSAPROTOCOL_INFOW protocolInfo;
    std::memset(&protocolInfo, 0, sizeof(protocolInfo));

    int socket_error = WSADuplicateSocketW(socket_handle, processInfo.dwProcessId, &protocolInfo);
    if ( socket_error != 0 ) {
        qCritical() << "WSADuplicateSocketW failed: " << socket_error;
        emit commandFailed();
        return;
    }

    socket_file.write(reinterpret_cast<char*>(&protocolInfo), sizeof(protocolInfo));
    socket_file.flush();
    socket_file.close();

    DWORD resume_ret = ResumeThread(processInfo.hThread);
    if ( resume_ret == (DWORD)-1 ) {
        qCritical() << "Failed to resume thread " << processInfo.hThread << ", error " << GetLastError();
        emit commandFailed();
        return;
    }

    m_socket->close();
    emit commandSuccessful();
}

QDebug operator<<(QDebug d, const SlaveShareFolderWithVM &folder)
{
    d << "SlaveShareFolderWithVM(folder = " << folder.folder() << ")";
    return d;
}
