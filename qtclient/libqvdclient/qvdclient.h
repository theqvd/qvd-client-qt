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

#include "qvdclient_global.h"

class LIBQVDCLIENT_EXPORT QVDClient : public QObject
{
    Q_OBJECT
public:
    enum class VMState {
        Stopped,
        Starting,
        Running
    };

    /**
     * @brief Type of second factor
     */
    enum class SecondFactorType {
        None,
        TOTP
    };

    /**
     * @brief Data for enrolling in the second factor
     */
    typedef struct {
        /**
         * @brief Type of second factor
         */
        SecondFactorType Type;

        /**
         * @brief Second factor issuer
         */
        QString          Issuer;

        /**
         * @brief Username we tried to log in as
         */
        QString          Login;

        /**
         * @brief TOTP secret, or similar
         */
        QString          Secret;

        /**
         * @brief OTP auth URL
         * See https://github.com/google/google-authenticator/wiki/Key-Uri-Format
         */
        QUrl             AuthURL;

        /**
         * @brief QR Code picture
         *
         * The server may provide us with the picture of the QR code directly, for our convenience.
         * It should be a PNG, and correspond to the AuthURL above.
         *
         * All the data contained here is encoded in the other class members, so this can be safely ignored and recreated locally if wanted.
         */
        QByteArray       QrPicture;

        /**
         * @brief helpMessage
         *
         * The server may provide an admin-configured message to help users obtain help when having trouble with authentication.
         * This may contain a phone number, an email address, or some other way to contain support.
         *
         * The message may contain HTML.
         */
        QString          helpMessage;

    } SecondFactorEnrollmentData;

    typedef struct VMInfo {
        QString name;
        int     id;
        bool    blocked;
        VMState state;
    } VMInfo;

    typedef enum class ConnectionError {
        None,
        AuthenticationError,  // bad pw
        SecondFactorRequired, // Server wants 2FA
        Timeout,              // command is taking too long
        VMStartError,         // vm failed to start
        ProtocolError,        // error in the protocol -- bad JSON for example
        ServerBlocked,        // Server has been blocked by the admin
        ServerError,          // Server returns an error
        XServerError,         // X server failed to start
        BackendError,         // Backend failed
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
    void stopVM(int id);
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
     * @brief vmPoweredDown
     * @param id
     * A VM was powered down
     */
    void vmPoweredDown(int id);

    /**
     * @brief connectionCompleted
     * A VM was connected to. The user should be looking at a functional desktop.
     */
    void vmConnected(int id);

    /**
     * @brief connectionError
     * @param error Error code due to which the connection failed
     * @param error_text Error description
     * @param headers Headers sent with the error. This is used for TOTP enrollment.
     */
    void connectionError(QVDClient::ConnectionError error, const QString& error_text, const QMap<QString, QString> headers);

    /**
     * @brief The server we tried to log into requires two factor authentication, but we didn't provide it.
     *
     * This event may be followed by twoFactorEnrollment, and will always be followed by connectionError.
     * @param type Type of second factor wanted
     * @param min_length Minimum length
     * @param max_length Maximum length
     */
    void twoFactorAuthenticationRequired(SecondFactorType type, int min_length, int max_length);

    /**
     * @brief The user or admin enabled TOTP. Server is providing us the data to enroll in the second factor from the client
     * @param data TOTP enrollment data
     *
     * This event will always be followed by connectionError.
     */
    void twoFactorEnrollment(const SecondFactorEnrollmentData &data);

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
    void qvd_vmPoweredDown();
    void qvd_vmConnected();
    void qvd_pong();


    void qvd_socketError(QAbstractSocket::SocketError  error);
    void qvd_hostFound();
    void qvd_socketStateChanged(QAbstractSocket::SocketState socketState);

    void backend_listeningOnTcp(QVDBackend::NXChannel channel, quint16 port);
    void backend_connectionEstablished();
    void backend_failed(QVDBackend::BackendError error, const QString &description);

    void slave_success(const QVDSlaveCommand &cmd);
    void slave_failure(const QVDSlaveCommand &cmd);
    void slave_done();

public slots:

    void disconnectFromQVD();


};

Q_DECLARE_METATYPE(QVDClient::VMState)
Q_DECLARE_METATYPE(QVDClient::SecondFactorType)

#endif // QVDCLIENT_H
