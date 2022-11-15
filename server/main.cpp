#include <QCoreApplication>
#include "server.h"
#include "database.h"
#include <QSqlError>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    database.setUserName("yaim");
    database.setPassword("yaim");
    database.setDatabaseName("yaim");
    if (database.open()){
        qDebug() << "连接成功";
    }
    else{
        qDebug() << database.lastError();
    }
    qDebug() << QSqlDatabase::drivers();
    Server server;
    return a.exec();
}
