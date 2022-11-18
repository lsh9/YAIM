#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>

class Database : public QSqlDatabase{

private:
    Database();

private:


public:
    bool addFriend();
    bool addGroup();

};

extern QSqlDatabase database;

#endif // DATABASE_H
