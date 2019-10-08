#ifndef QVDNXBACKEND_H
#define QVDNXBACKEND_H

#include <QObject>
#include <QProcess>
#include <QTcpServer>

#include "qvdbackend.h"
#include "socketforwarder.h"

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

private:
    QString m_nxproxyBinary = "/usr/bin/nxproxy";

    QTcpSocket *m_qvd_connection_socket = nullptr;
    SocketForwarder *m_forwarder = nullptr;
    QTcpServer m_proxy_listener;

    QProcess m_process;
};

#endif // QVDNXBACKEND_H
