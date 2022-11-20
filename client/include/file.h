#ifndef FILE_H
#define FILE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>

class File{
private:
    File() = default;

private:
    static QDir yaim_dir;
    static QSqlDatabase m_db;

public:
    static QString getYaimPath();
    static QString getDatabasePath(const QString &username);
    static QString getUserPath(const QString& username);
    static QSqlDatabase& getUserDatabase(const QString& username);

private:
    static void createTable(const QString& tablename, const QString& create_sql);
};


#endif // FILE_H
