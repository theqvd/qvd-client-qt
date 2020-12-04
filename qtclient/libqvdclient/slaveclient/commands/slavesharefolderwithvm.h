#ifndef SLAVESHAREFOLDERWITHVM_H
#define SLAVESHAREFOLDERWITHVM_H

#include <QObject>
#include <QTemporaryFile>

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
    ~SlaveShareFolderWithVM();

    virtual void run();

    Error error() const { return m_error; }

    QString folder() const { return m_folder; }
    void setFolder(const QString &folder) { m_folder = folder; }

    QString sftpServerBinary() const { return m_sftp_server_binary; }
    void setSftpServerBinary(const QString &sftp_server_binary) { m_sftp_server_binary = sftp_server_binary; }

    bool enableDebug() const { return m_enable_debug; }
    void setEnableDebug(bool enable_debug) { m_enable_debug = enable_debug; }

private slots:
    void http_finished();

private:
    void setError(const Error &error) { m_error = error; }
    Error m_error;

    QString m_folder;
    QString m_sftp_server_binary = "/usr/lib/openssh/sftp-server";
    bool m_enable_debug = true;
};


QDebug operator<< (QDebug d, const SlaveShareFolderWithVM &folder);

#endif // SLAVESHAREFOLDERWITHVM_H
