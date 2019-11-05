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

/**
 * @brief libXcomp based NX backend.
 *
 * This backend uses libXcomp to implement NX support. Compared to the process-based
 * QVDNXBackend, it has much greater access to the internals of NX.
 *
 * \par Issues with SSL
 *
 * QVD uses a single SSL protected connection to handle its own protocol that is later
 * handed to libXcomp. Unfortunately, libXcomp doesn't currently have any way of making
 * it call a function to obtain data, and can't work with a SSL socket, so a workaround
 * had to be implemented.
 *
 * The SocketForwarder class connects two sockets together by reading from one and writing
 * to another and vice-versa. This is used to connect a SSL socket with a cleartext one, and
 * then hand the cleartext side to libXcomp.
 *
 * \par Initialization sequence:
 * * Start X server, if needed
 * * Start local socket listener
 * * Connect QVD socket (SSL) with local socket (cleartext)
 * * Hand cleartext side to libXcomp
 * * Do NX processing.
 */

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

    // Stage 3: Run NX.
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
