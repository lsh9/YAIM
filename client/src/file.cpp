#include "file.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>

QDir File::yaim_dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/YAIM");
QSqlDatabase File::m_db = QSqlDatabase::addDatabase("QSQLITE");

QString File::getYaimPath()
{
    if(!yaim_dir.exists()){
        yaim_dir.mkdir("");
    }
    return yaim_dir.absolutePath();
}

QString File::getDatabasePath(const QString &username)
{
    return getUserPath(username) + username + ".db";
}

QString File::getUserPath(const QString &username)
{
    if (!yaim_dir.exists(username)){
        yaim_dir.mkdir(username);
    }
    return yaim_dir.absoluteFilePath(username) + "/";
}

QSqlDatabase& File::getUserDatabase(const QString &username)
{
    // create database
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(File::getDatabasePath(username));
    }

    // open database and check tables
    if (!m_db.open()) {
        qDebug() << "Error: Failed to connect database." << m_db.lastError();
    }
    createTable("message", "sender TEXT, receiver TEXT, content TEXT, time TEXT");
    createTable("friend", "friend TEXT");
    return m_db;
}

void File::createTable(const QString& tablename, const QString& create_constraint)
{
    QSqlQuery sql_query(m_db);
    QString sql_query_table = "select count(*) from sqlite_master where type='table' and name='%1'";
    sql_query.exec(sql_query_table.arg(tablename));
    if(sql_query.value(0).toInt() == 0){
        if(!sql_query.exec(QString("CREATE TABLE '%1' (%2);").arg(tablename).arg(create_constraint))) {
            qDebug() << "Error: Fail to create table: " << tablename << sql_query.lastError();
        }
    }
}
