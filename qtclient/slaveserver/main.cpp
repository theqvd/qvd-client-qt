#include <QCoreApplication>
#include <QtHttpServer>
#include "socketforwarder.h"
#include <QTcpSocket>
#include <QDebug>

#include <stdio.h>

SocketForwarder *g_forwarder = nullptr;

QTcpSocket socket;
QTcpSocket stdin_socket;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QHttpServer httpServer;

    httpServer.route("/", []() { return "Server ready.\n"; });
    httpServer.route("/ping", []() { return "Pong!\n"; });
    httpServer.route("/version", []() { return "4.2\n"; });

    QObject::connect( &httpServer, &QHttpServer::missingHandler, [](const QHttpServerRequest &req, QTcpSocket *sock) {
        qCritical() << "Missing handler for request " << req;
    });



    // QHttpServer can't listen on stdin, so we work around it here.
    const auto port = httpServer.listen(QHostAddress::LocalHost);
    socket.connectToHost("127.0.0.1", port);
    if ( stdin_socket.setSocketDescriptor(0) ) {
        qInfo() << "Socket connected to stdin";
    } else {
        qCritical() << "Failed to connect socket to stdin, aborting.";
        a.exit(1);
    }

    QObject::connect(&socket, &QTcpSocket::connected, [&]() {
        qInfo() << "Connected!";
        g_forwarder = new SocketForwarder(nullptr, stdin_socket, socket);
    });

    QObject::connect(&socket, &QTcpSocket::errorOccurred, [&]() {
        qCritical() << "Failed to connect to port " << port;
        a.exit(2);
    });

    qInfo() << "Looping, listening on port " << port;
    return a.exec();
}
