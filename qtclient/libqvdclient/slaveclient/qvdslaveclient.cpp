#include "qvdslaveclient.h"

#include <QException>
#include <QTcpSocket>


#include "commands/slavesharefolderwithvm.h"
#include "commands/shareusbdevice.h"
#include "commands/connectaudio.h"
#include "usbip/usbdevice.h"


QVDSlaveClient::QVDSlaveClient(quint16 port, const QString &slave_key) : QObject()
{
    m_port = port;
    m_slave_key = slave_key;


    connect(&m_socket, &QTcpSocket::connected, this, &QVDSlaveClient::socket_connected );

    m_socket.connectToHost("localhost", m_port);
}

QVDSlaveClient::~QVDSlaveClient()
{
    if ( m_command != nullptr ) {
        m_command->stop();
        m_command->deleteLater();
    }
}


quint16 QVDSlaveClient::port() const
{
    return m_port;
}


QString QVDSlaveClient::slaveKey() const
{
    return m_slave_key;
}

void QVDSlaveClient::socket_connected()
{
    runCommand();
}

void QVDSlaveClient::command_successful()
{
    emit commandSuccessful(*m_command);
    emit finished();
}

void QVDSlaveClient::command_failed()
{
    emit commandFailed(*m_command);
    emit finished();
}

void QVDSlaveClient::setupSignals(QVDSlaveCommand *cmd)
{
    connect( cmd, &QVDSlaveCommand::commandSuccessful, this, &QVDSlaveClient::command_successful );
    connect( cmd, &QVDSlaveCommand::commandFailed, this, &QVDSlaveClient::command_failed );
}

void QVDSlaveClient::runCommand()
{
    if ( m_socket.state() == QAbstractSocket::ConnectedState && m_command && !m_command_executed) {
        m_command_executed = true;

        m_command->init(&m_socket, m_slave_key);
        m_command->run();
    }
}

void QVDSlaveClient::stopCommand()
{
    if ( m_command != nullptr ) {
        m_command->stop();
    }
}



void QVDSlaveClient::shareFolderWithVM(const QString &local_path)
{
    if ( m_command != nullptr ) {
        throw BusyException();
    }

    m_command = new SlaveShareFolderWithVM(local_path);
    setupSignals(m_command);
    runCommand();
}



void QVDSlaveClient::mountVMFolder([[maybe_unused]] const QString &remote_path, [[maybe_unused]] const QString &local_mountpoint)
{

}

void QVDSlaveClient::shareUsbWithVM(const USBDevice &device)
{
    if ( m_command != nullptr ) {
        throw BusyException();
    }

    m_command = new ShareUSBDevice(device);
    setupSignals(m_command);
    runCommand();
}

void QVDSlaveClient::openFileOnVM([[maybe_unused]] const QString &remote_relative_path)
{

}

void QVDSlaveClient::connectCompressedAudio(quint16 audio_port)
{
    if ( m_command != nullptr ) {
        throw BusyException();
    }

    m_command = new ConnectAudio(audio_port);
    setupSignals(m_command);
    runCommand();
}

