//#include "database.h"
//#include <QDebug>

//Database::Database(QString username) : QSqlDatabase(), m_username(username) {}

//Database::~Database() {}

//bool Database::connect() {
//	m_db = QSqlDatabase::addDatabase("QSQLITE");
//	m_db.setDatabaseName(m_username + ".db");
//	if (!m_db.open()) {
//		qDebug() << "Error: connection with database fail";
//		return false;
//	} else {
//		qDebug() << "Database: connection ok";
//		return true;
//	}
//}

//bool Database::disconnect() {
//	m_db.close();
//	return true;
//}

//bool Database::isConnected() {
//	return m_db.isOpen();
//}

//bool Database::createTable(const QString& tablename,
//						   const QString& create_constraint) {
//	QSqlQuery sql_query(m_db);
//	QString sql_query_table =
//		"select count(*) from sqlite_master where type='table' and name='%1'";
//	sql_query.exec(sql_query_table.arg(tablename));
//	if (sql_query.value(0).toInt() == 0) {
//		if (!sql_query.exec(QString("CREATE TABLE '%1' (%2);")
//								.arg(tablename)
//								.arg(create_constraint))) {
//			qDebug() << "Error: Fail to create table: " << tablename
//					 << sql_query.lastError();
//		}
//	}
//}

//bool Database::insertData(QString name, QString surname, QString email) {
//	QSqlQuery query;
//	query.prepare(
//		"INSERT INTO users (name, surname, email) VALUES (:name, :surname, "
//		":email)");
//	query.bindValue(":name", name);
//	query.bindValue(":surname", surname);
//	query.bindValue(":email", email);
//	if (!query.exec()) {
//		qDebug() << "insertion into table failed: " << query.lastError();
//		return false;
//	} else {
//		qDebug() << "insertion into table succeeded";
//		return true;
//	}
//}
