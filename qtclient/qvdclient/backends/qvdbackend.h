#ifndef QVDBACKEND_H
#define QVDBACKEND_H

#include <QObject>
#include <QTcpSocket>

#include "qvdconnectionparameters.h"

/**
 * @brief QVD backend base class
 *
 * This is the base for QVD backends.
 */
class QVDBackend : public QObject
{
    Q_OBJECT
public:
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
     * @brief Backend has finished
     */
    void finished();
public slots:

private:
    QVDConnectionParameters m_parameters;

};

#endif // QVDBACKEND_H
