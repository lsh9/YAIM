#include "server.h"


Server::Server()
    :server_socket(new QTcpServer())
{
    server_socket->listen(QHostAddress::Any, 8080);
    connect(server_socket, &QTcpServer::newConnection, this, [=]{
        QTcpSocket* client_socket = server_socket->nextPendingConnection();
        Client* client = new Client(client_socket);
        clients.append(client);
        client->start();
    });
}

Server::~Server()
{
    server_socket->close();
    delete server_socket;
    for (auto& client: clients){
        delete client;
    }
}
