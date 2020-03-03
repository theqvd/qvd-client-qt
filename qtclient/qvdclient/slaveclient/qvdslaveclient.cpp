#include "qvdslaveclient.h"

#include <QException>
#include <QTcpSocket>


#include "commands/slavesharefolderwithvm.h"


QVDSlaveClient::QVDSlaveClient(quint16 port, const QString &slave_key) : QObject()
{
    m_port = port;
    m_slave_key = slave_key;


    connect(&m_socket, &QTcpSocket::connected, this, &QVDSlaveClient::socket_connected );

    m_socket.connectToHost("localhost", m_port);
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



void QVDSlaveClient::shareFolderWithVM(const QString &local_path)
{
    if ( m_command != nullptr ) {
        throw BusyException();
    }

    m_command = new SlaveShareFolderWithVM(local_path);
    setupSignals(m_command);
    runCommand();
}



void QVDSlaveClient::mountVMFolder(const QString &remote_path, const QString &local_mountpoint)
{

}

void QVDSlaveClient::shareUsbWithVM(const QString &device)
{

}

void QVDSlaveClient::openFileOnVM(const QString &remote_relative_path)
{

}

