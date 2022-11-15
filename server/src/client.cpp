#include "client.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


Client::Client(QTcpSocket* socket, QObject *parent)
    :QThread(parent), client_socket(socket)
{
    qDebug() <<"successfully create a client by thread:" << currentThreadId();
    connect(client_socket, &QTcpSocket::readyRead, this, &Client::readMessage);
}

Client::~Client()
{
    client_socket->close();
    delete client_socket;
}

void Client::run()
{
    qDebug() << "running sub thread:" << currentThreadId();
}

void Client::readMessage()
{
    static int count = 0;
    static int length = 0;
    static int command = 0;
    static QString all;
    if (count == 0){
        client_socket->read((char*)&command, 4);
        client_socket->read((char*)&length, 4);
    }
    QByteArray remain = client_socket->readAll();
    count += remain.size();
    all += remain;
    if (count == length){
        count = 0;
        length = 0;
        all.resize(0);
        QJsonDocument jsonDocument = QJsonDocument::fromJson(all.toLocal8Bit().data());
        if( jsonDocument.isNull() ){
            qDebug() << "===> QJsonDocument: "<< all.toLocal8Bit().data();
        }
        else {
            qDebug() << "successfully recerved data!";
        }
        QJsonObject jsonObject = jsonDocument.object();
    }
}
