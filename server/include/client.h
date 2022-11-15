#ifndef CLIENT_H
#define CLIENT_H

/**
每个Client在其线程中处理相应客户端的信息
*/

#include <QTcpSocket>
#include <QThread>

class Client : public QThread{
    Q_OBJECT

public:
    Client(QTcpSocket* socket, QObject *parent=nullptr);
    ~Client();

    void run();

private slots:
    void readMessage();

private:
    QTcpSocket* client_socket;
};

#endif // CLIENT_H
