#ifndef QVDLIBNXBACKEND_H
#define QVDLIBNXBACKEND_H

#include <QObject>
#include <QProcess>
#include <QTcpServer>
#include <QTimer>
#include <QLocalServer>
#include <QLocalSocket>


#include "qvdbackend.h"
#include "socketforwarder.h"
#include "xserverlauncher.h"


class QVDLibNXBackend : public QVDBackend
{
    Q_OBJECT
public:
    QVDLibNXBackend(QObject *parent = nullptr);
    virtual ~QVDLibNXBackend() override;

    virtual void start(QTcpSocket *socket) override;
    virtual void stop() override;
    virtual int64_t bytesRead() override;
    virtual int64_t bytesWritten() override;

private slots:

    // Stage 1: X server is available
    void XServerReady();

    // Stage 2: Socket server for the unencrypted socket is available
    void SocketServerReady();

    void RunNX();
private:
    void startNX();


    QTcpSocket *m_qvd_connection_socket = nullptr;
    XServerLauncher m_x_server_launcher;
    QTimer m_nx_timer;

    SocketForwarder *m_forwarder = nullptr;
    QLocalServer m_unencrypted_socket_server;
    QLocalSocket m_unencrypted_socket_client;

    QTcpServer m_proxy_listener;

};
#endif // QVDLIBNXBACKEND_H
