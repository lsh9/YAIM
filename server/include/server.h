#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "client.h"

// 服务器类，可以使用单例模式
class Server : public QObject{
    Q_OBJECT

public:
    Server();
    ~Server();

private:
    QTcpServer* server_socket;
    QList<Client*> clients;
};


#endif // SERVER_H
