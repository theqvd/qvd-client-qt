#ifndef QVDCLIENT_H
#define QVDCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QSize>
#include <QTcpServer>
#include <QProcess>
#include <QSslCertificate>
#include "QSettings"

#include "qvdhttp.h"
#include "qvdconnectionparameters.h"
#include "socketforwarder.h"
#include "backends/qvdbackend.h"
#include "slaveclient/qvdslaveclient.h"



class QVDClient : public QObject
{
    Q_OBJECT
public:
    enum VMState {
        Stopped,
        Starting,
        Running
    };

    typedef struct VMInfo {
        QString name;
        int     id;
        bool    blocked;
        VMState state;
    } VMInfo;

    typedef enum ConnectionError {
        None,
        AuthenticationError,  // bad pw
        Timeout,              // command is taking too long
        VMStartError,         // vm failed to start
        ProtocolError,        // error in the protocol -- bad JSON for example
        ServerBlocked,        // Server has been blocked by the admin
        ServerError,          // Server returns an error
        Unexpected            // unforseen error that is not specifically handled

    } ConnectionError;

private:

    QSslSocket *m_socket = Q_NULLPTR;
    QVDHTTP *m_http = Q_NULLPTR;

    QVDBackend *m_backend = nullptr;



    QNetworkRequest createRequest(const QUrl &url);

    QVDConnectionParameters m_parameters;

    bool checkReply(QVDNetworkReply *reply);
    QVDSlaveClient *createSlaveClient();
    QList<QVDSlaveClient*> m_active_slave_clients;
    QString m_slave_key; // Slave key used for slave channel authentication
    quint16 m_slave_port = 0;
public:


    explicit QVDClient(QObject *parent = nullptr);


    QString getNXProxy() const;

    void setNXProxy(const QString &proxy);

    /**
     * @brief Returns the socket being used for the connection
     * @return The socket being used for the connection
     *
     * This function is mainly intended for retrieving socket status and error information.
     */
    QTcpSocket *getSocket();

    QSettings settings_cert; //Declaración global

    void connectToQVD();
    void requestVMList();
    void ping();
    void connectToVM(int id);



    void handle_printer();



    QVDConnectionParameters getParameters() const;
    void setParameters(const QVDConnectionParameters &parameters);

    QVDBackend *getBackend() const;
    void setBackend(QVDBackend *backend);





signals:
    /**
     * @brief Connected to the QVD server
     * This is the first connection stage: a connection was made to the port.
     */
    void connectionEstablished();

    /**
     * @brief connectionCompleted
     * A VM was connected to. The user should be looking at a functional desktop.
     */
    void vmConnected();

    void connectionError(QVDClient::ConnectionError error, QString error_text);


    /**
     * @brief Connection terminated
     *
     * The connection to the QVD server was closed, either due to the user disconnecting,
     * or an error.
     */
    void connectionTerminated();

    /**
     * @brief Virtual Machine list received
     * @param vm_list List of virtual machines
     *
     * If there's more than one machine that can be connected to, this event will be emitted.
     *
     */
    void vmListReceived(const QList<QVDClient::VMInfo> &vm_list);
    void socketError(QAbstractSocket::SocketError  error);
    void sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain, bool &continue_connection);
    void hostFound();
    void socketStateChanged(QAbstractSocket::SocketState socketState);



private slots:
    void qvd_sslErrors(const QList<QSslError> &errors);
    void qvd_connectionEstablished();
    void qvd_vmListDownloaded();
    void qvd_vmProcessing();
    void qvd_vmConnected();
    void qvd_pong();


    void qvd_socketError(QAbstractSocket::SocketError  error);
    void qvd_hostFound();
    void qvd_socketStateChanged(QAbstractSocket::SocketState socketState);

    void backend_listeningOnTcp(QVDBackend::NXChannel channel, quint16 port);
    void backend_connectionEstablished();

    void slave_success(const QVDSlaveCommand &cmd);
    void slave_failure(const QVDSlaveCommand &cmd);
    void slave_done();

public slots:

    void disconnectFromQVD();


};

#endif // QVDCLIENT_H
