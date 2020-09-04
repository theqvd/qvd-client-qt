#ifndef QVDSLAVECLIENT_H
#define QVDSLAVECLIENT_H

#include <QObject>
#include <QException>

#include "commands/qvdslavecommand.h"
#include "usbip/usbdevice.h"

/**
 * @brief The QVDSlaveClient class
 *
 * This class talks to the slave client interface. It connects to the slave port
 * on localhost, and NX forwards the connection to the VM side.
 *
 * This connects to a small HTTP server that authenticates requests and performs
 * multiplexing by performing different actions depending on the request used.
 *
 * Actions are performed asynchronously.
 *
 * @warning This class is single use. Once a method that performs an action is
 * called, the instance is dedicated to that one function and nothing else. To
 * perform more actions, create more instances.
 */
class QVDSlaveClient : public QObject
{
    Q_OBJECT
public:



    /**
     * @brief The BusyException class
     *
     * Once a QVDSlaveClient instance starts doing something, it's forever dedicated
     * to that particular function and will throw this exception if anything further
     * is attempted.
     *
     * To perform more actions, create more instances of QVDSlaveClient.
     */
    class BusyException : public QException {

    };

    /**
     * @brief QVDSlaveClient
     * @param port Port to which to connect
     * @param slave_key Authentication key
     *
     * The class will attempt to connect to the port immediately, even
     * before any command is given.
     */
    QVDSlaveClient(quint16 port, const QString &slave_key);

    /**
     * @brief shareFolderWithVM
     * @param local_path
     *
     * Share a local folder with the VM, starting a SFTP server on the
     * client side.
     *
     * The folder will be mounted at a place decided by the VM, usually
     * ~/Redirected/<name>
     *
     * This function begins the request and returns immediately.
     */
    void shareFolderWithVM(const QString &local_path);

    /**
     * @brief mountVMFolder
     * @param remote_path
     * @param local_mountpoint
     *
     * Mount a remote directly on the local machine, using sshfs.
     *
     * This function begins the request and returns immediately.
     */
    void mountVMFolder(const QString &remote_path, const QString &local_mountpoint);

    /**
     * @brief shareUsbWithVM
     * @param device Device to share
     *
     * Share a local USB device with the VM
     *
     * This function begins the request and returns immediately.
     */
    void shareUsbWithVM(const USBDevice &device);

    /**
     * @brief openFileOnVM
     * @param remote_relative_path
     *
     * Open a file on the desktop in the VM, using xdg-open or similar.
     *
     * This function begins the request and returns immediately.
     */
    void openFileOnVM(const QString &remote_relative_path);



    quint16 port() const;
    QString slaveKey() const;

signals:
    /**
     * @brief commandSuccessful
     * @param command The command that was executed
     *
     * Emitted when a command successfully runs to completion
     */
    void commandSuccessful(const QVDSlaveCommand &command);

    /**
     * @brief commandFailed
     * @param command The command that was executed
     *
     * Emitted when a command failed to complete
     */
    void commandFailed(const QVDSlaveCommand &command);

    /**
     * @brief finished
     *
     * Emitted when the command has finished, successfully or not.
     */
    void finished();

private slots:
    void socket_connected();
    void command_successful();
    void command_failed();

private:
    void setupSignals(QVDSlaveCommand *cmd);

    void runCommand();

    quint16 m_port = 12040;
    QString m_slave_key = "";

    QVDSlaveCommand *m_command = nullptr;
    bool m_command_executed = false;
    QTcpSocket m_socket;
};

#endif // QVDSLAVECLIENT_H
