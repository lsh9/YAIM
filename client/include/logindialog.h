#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QString>
#include <QThread>

#include "sio_client.h"
#include "defines.h"

using namespace sio;

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class LoginDialog : public QDialog {
	Q_OBJECT

   public:
	LoginDialog(QWidget* parent = nullptr);
	~LoginDialog();

   Q_SIGNALS:
	void requestLogin(ResponseObject);
	void requestRegister(ResponseObject);

   private Q_SLOTS:
	void on_pushButtonLogin_clicked();
	void on_pushButtonRegister_clicked();

   private:
	bool getInput();
	void connectServer();
	void login(ResponseObject);
	void registration(ResponseObject);

   private:
	Ui::Login* ui;
	QString username;
	QString password;
    QString nickname;
	std::unique_ptr<client> cli;
};

#endif	// LOGINDIALOG_H
