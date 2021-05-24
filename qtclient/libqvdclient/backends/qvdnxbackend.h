#ifndef QVDNXBACKEND_H
#define QVDNXBACKEND_H

#include <QObject>
#include <QProcess>
#include <QTcpServer>
#include <QStringBuilder>

#include "qvdbackend.h"
#include "socketforwarder.h"
#include "xserverlauncher.h"

#include "helpers/linebuffer.h"
#include "qvdconnectionparameters.h"
#include "qvdclient_global.h"

class LIBQVDCLIENT_EXPORT QVDNXBackend : public QVDBackend
{
    Q_OBJECT
public:
    QVDNXBackend(QObject *parent = nullptr);
    virtual ~QVDNXBackend() override;

    QString nxproxyBinary() const;
    void setNxproxyBinary(const QString &nxproxy_binary);

    QVDConnectionParameters getParameters() const;
    void setParameters(const QVDConnectionParameters &parameters);

    virtual void start(QTcpSocket *socket) override;
    virtual void stop() override;
    virtual int64_t bytesRead() override;
    virtual int64_t bytesWritten() override;


private slots:
    void connectionAccepted();

    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    void processStdoutReady();
    void processStderrReady();

    void XServerReady();
    void XServerFailed(const QString &error);

private:
    QString m_nxproxyBinary = "/usr/bin/nxproxy";

    QTcpSocket *m_qvd_connection_socket = nullptr;
    SocketForwarder *m_forwarder = nullptr;
    QTcpServer m_proxy_listener;



    QVDConnectionParameters m_parameters;

    XServerLauncher m_x_server_launcher;

    LineBuffer m_buffer;
    //QString m_buffer; // Buffer for nxproxy output, for output parsing


    int m_nx_proxy_port_offset = 4000; // DEFAULT_NX_PROXY_PORT_OFFSET in nx-libs

    // QProcess may emit events during destruction, such as handling the last messages emitted by the process.
    // The last declared member will be the first to be destroyed, so QProcess has to be last here, ensuring
    // the rest of the class is still there to deal with any last-time events.
    QProcess m_process;
};

#endif // QVDNXBACKEND_H
