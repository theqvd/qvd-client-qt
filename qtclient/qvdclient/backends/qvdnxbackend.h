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

class QVDNXBackend : public QVDBackend
{
    Q_OBJECT
public:
    QVDNXBackend(QObject *parent = nullptr);
    virtual ~QVDNXBackend() override;



    QString nxproxyBinary() const;
    void setNxproxyBinary(const QString &nxproxy_binary);

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
private:
    QString m_nxproxyBinary = "/usr/bin/nxproxy";

    QTcpSocket *m_qvd_connection_socket = nullptr;
    SocketForwarder *m_forwarder = nullptr;
    QTcpServer m_proxy_listener;

    QProcess m_process;

    XServerLauncher m_x_server_launcher;


    LineBuffer m_buffer;
    //QString m_buffer; // Buffer for nxproxy output, for output parsing


    int m_nx_proxy_port_offset = 4000; // DEFAULT_NX_PROXY_PORT_OFFSET in nx-libs

};

#endif // QVDNXBACKEND_H
