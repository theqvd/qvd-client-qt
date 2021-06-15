#ifndef SOCKETFORWARDER_H
#define SOCKETFORWARDER_H

#include <QObject>
#include <QIODevice>
#include <inttypes.h>

class SocketForwarder : public QObject
{
    Q_OBJECT

private:
    QIODevice &m_socket_a;
    QIODevice &m_socket_b;
    int64_t bytes_a_to_b = 0;
    int64_t bytes_b_to_a = 0;

public:
    SocketForwarder(QObject *parent, QIODevice &a, QIODevice &b);
    ~SocketForwarder();

    int64_t getBytesAtoB() const;
    int64_t getBytesBtoA() const;

signals:
    /**
     * @brief The total count of bytes transferred has been updated
     * @param total_bytes_a_to_b
     * @param total_bytes_b_to_a
     */
    void totalStatsUpdated(int64_t total_bytes_a_to_b, int64_t total_bytes_b_to_a);

    /**
     * @brief Some data has been transferred through the forwarder.
     *
     * This is a relative measure, counting data since the previous event.
     * @param transferred_a_to_b
     * @param transferred_b_to_a
     */
    void dataTransferred(int64_t transferred_a_to_b, int64_t transferred_b_to_a);
public slots:
    void socketA_readyRead();
    void socketB_readyRead();

    void socketA_finished();
    void socketB_finished();

};

#endif // SOCKETFORWARDER_H
