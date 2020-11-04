#ifndef QVDSLAVECOMMAND_H
#define QVDSLAVECOMMAND_H

#include <QObject>
#include <QAbstractSocket>
#include <QNetworkRequest>
#include <QUrl>
#include <qvdhttp.h>


/**
 * @brief The QVDSlaveCommand class
 *
 * Base class for all slave command classes.
 */
class QVDSlaveCommand : public QObject
{
    Q_OBJECT
public:

    QVDSlaveCommand(QObject *parent = nullptr) : QObject(parent) {}

    virtual ~QVDSlaveCommand();


    /**
     * @brief Initializes the class
     * @param socket
     * @param slave_key
     *
     * Sets the socket through which communication will be done,
     * and the slave key.
     */
    void init(QTcpSocket *socket, const QString &slave_key);

    /**
     * @brief run
     *
     * Performs the action
     */
    virtual void run() = 0;

signals:
    void commandSuccessful();
    void commandFailed();

protected:
    QNetworkRequest createRequest(const QUrl &url);
    QVDHTTP *m_http = nullptr;
    QTcpSocket *m_socket = nullptr;
    QString m_slave_key = "";
};

QDebug operator<< (QDebug d, const QVDSlaveCommand &cmd);

#endif // QVDSLAVECOMMAND_H
