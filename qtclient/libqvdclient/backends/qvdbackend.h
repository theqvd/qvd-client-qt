#ifndef QVDBACKEND_H
#define QVDBACKEND_H

#include <QObject>
#include <QTcpSocket>

#include "qvdconnectionparameters.h"
#include "qvdclient_global.h"


/**
 * @brief QVD backend base class
 *
 * This is the base for QVD backends.
 */
class LIBQVDCLIENT_EXPORT QVDBackend : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        Unknown,
        X11,
        X11Aux,
        CUPS,
        Multimedia,
        SMB,
        HTTP,
        Font,
        File,
        Slave
    } NXChannel;
    Q_ENUM(NXChannel)

    /**
     * @brief Error type
     */
    typedef enum {
        UnknownError,
        NotFound,
        LinkingError,
        Crash,
        Abort,
        StartFailure,
        CommunicationFailure,
        XServerFailed,
    } BackendError;
    Q_ENUM(BackendError)

    explicit QVDBackend(QObject *parent = nullptr);
    ~QVDBackend();



    QVDConnectionParameters parameters() const;
    void setParameters(const QVDConnectionParameters &parameters);


    /**
     * @brief Starts the backend
     * @param socket Connected socket on which the backend will run
     */
    virtual void start(QTcpSocket *socket) = 0;

    /**
     * @brief Stops the backend
     */
    virtual void stop() = 0;

    /**
     * @brief Number of bytes read by the backend from the server
     * @return Byte count
     */
    virtual int64_t bytesRead();

    /**
     * @brief Number of bytes written by the backend to the server
     * @return Byte count
     */
    virtual int64_t bytesWritten();

    quint16 cupsPort() const;
    void setCupsPort(const quint16 &cupsPort);

    quint16 slavePort() const;
    void setSlavePort(const quint16 &slavePort);

    quint16 audioPort() const;
    void setAudioPort(const quint16 &audioPort);

signals:

    /**
     * @brief Backend started processing data
     */
    void started();

    /**
     * @brief stdout from the backend process, if there is one.
     * @param data Raw data from the backend.
     */
    void standardOutput(const QByteArray &data);

    /**
     * @brief stderr from the backend process, if there is one.
     * @param data Raw data from the backend.
     */
    void standardError(const QByteArray &data);

    /**
     * @brief forwardingRemoteConnectionsToPort
     * @param channel NX channel that will be forwarded
     * @param host Host where the backend will connect
     * @param port Port on the indicated host
     *
     * Backend is forwarding something happening on the VM to the local side.
     * When a connection is made on the VM side, the backend will forward it to
     * the indicated host and port on the local side.
     */
    void forwardingRemoteConnectionsToTcpPort(QVDBackend::NXChannel channel, const QString& host, quint32 port);

    /**
     * @brief forwardingRemoteConnectionsToUnixSocket
     * @param channel
     * @param unix_socket
     *
     * Backend is forwarding something happening on the VM to the local side.
     * When a connection is made on the VM side, the backend will forward it to
     * the indicated UNIX socket.
     */
    void forwardingRemoteConnectionsToUnixSocket(QVDBackend::NXChannel channel, const QString& unix_socket);

    /**
     * @brief forwardingRemoteConnectionsToDisplay
     * @param channel
     * @param host
     * @param display
     *
     * The backend is forwarding X11 connections to the specified display
     */
    void forwardingRemoteConnectionsToDisplay(QVDBackend::NXChannel channel, const QString& host, quint32 display);


    /**
     * @brief listeningOnPort
     * @param local_port Port where the backend is listening
     *
     * Backend has opened a listener on this port and will forward the connection
     * to the VM side.
     */
    void listeningOnTcpPort(QVDBackend::NXChannel channel, quint32 local_port);

    /**
     * @brief listeningOnUnixSocket
     * @param channel
     * @param unix_socket
     *
     * Backend is listening on the specified UNIX socket, and will forward the connection
     * to the VM side.
     *
     */
    void listeningOnUnixSocket(QVDBackend::NXChannel channel, const QString &unix_socket);


    /**
     * @brief connectionEstablished
     * Emitted when the initialization process has been completed, and the connection
     * has been established.
     */
    void connectionEstablished();


    /**
     * @brief connectionTerminated
     * The connection has been terminated.
     */
    void connectionTerminated();

    /**
     * @brief The backend has failed, and the connection has been closed
     * @param error Description of the error
     */
    void failed(BackendError error, const QString &description);


    /**
     * @brief Backend has finished
     */
    void finished();
public slots:

private:
    QVDConnectionParameters m_parameters;

    quint16 m_cups_port = 631;
    quint16 m_slave_port = 63640;
    quint16 m_audio_port = 4713;

protected:
    bool m_linking_error_detected = false;
};

#endif // QVDBACKEND_H
