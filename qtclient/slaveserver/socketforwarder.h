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

public slots:
    void socketA_readyRead();
    void socketB_readyRead();

    void socketA_finished();
    void socketB_finished();

};

#endif // SOCKETFORWARDER_H
