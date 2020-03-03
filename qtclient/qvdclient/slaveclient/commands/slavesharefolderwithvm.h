#ifndef SLAVESHAREFOLDERWITHVM_H
#define SLAVESHAREFOLDERWITHVM_H

#include <QObject>
#include "qvdslavecommand.h"

class SlaveShareFolderWithVM : public QVDSlaveCommand
{
public:
    typedef enum {
        OK,
        BinaryNotFound,
        BinaryNotExecutable,
        DirectoryNotFound
    } Error;

    SlaveShareFolderWithVM(const QString &folder) : QVDSlaveCommand(nullptr), m_folder(folder) {}

    virtual void run();

    Error error() const { return m_error; }

    QString folder() const { return m_folder; }
    void setFolder(const QString &folder) { m_folder = folder; }

    QString sftpServerBinary() const { return m_sftp_server_binary; }
    void setSftpServerBinary(const QString &sftp_server_binary) { m_sftp_server_binary = sftp_server_binary; }

private slots:
    void http_finished();

private:
    void setError(const Error &error) { m_error = error; }
    Error m_error;

    QString m_folder;
    QString m_sftp_server_binary = "/usr/lib/openssh/sftp-server";
};

#endif // SLAVESHAREFOLDERWITHVM_H
