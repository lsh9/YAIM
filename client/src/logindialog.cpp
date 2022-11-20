#include "logindialog.h"
#include "mainwindow.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QMessageBox>

#define kURL "ws://localhost:3000"
#ifdef WIN32
#define BIND_EVENT(IO, EV, FN)             \
	do {                                   \
		socket::event_listener_aux l = FN; \
		IO->on(EV, l);                     \
	} while (0)
#else
#define BIND_EVENT(IO, EV, FN) IO->on(EV, FN)
#endif

LoginDialog::LoginDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::Login), cli(new client()) {
	ui->setupUi(this);
	connect(this, &LoginDialog::requestSignIn, this, &LoginDialog::signin);
	connect(this, &LoginDialog::requestSignUp, this, &LoginDialog::signup);
}

LoginDialog::~LoginDialog() {
	cli->close();
	delete ui;
}

// 读取并判断用户名、密码框是否合法
bool LoginDialog::getInput() {
	//--------To Do: 增加输入判断，如命名规则等------
	username = ui->lineEditUsername->text();
	password = ui->lineEditPassword->text();
	if (username.isEmpty() || username.size() < 6) {
		QMessageBox::warning(this, "warning",
							 QString("Please input valid username!"));
		return false;
	}
	if (password.isEmpty() || password.size() < 6) {
		QMessageBox::warning(this, "warning",
							 QString("Please input valid password!"));
		return false;
	}
	return true;
}

void LoginDialog::connectServer() {
	if (!cli->opened()) {
		cli->connect(kURL);
		qDebug() << "session id: "
				 << QString::fromStdString(cli->get_sessionid());
	}
}

// 点击登录按钮响应事件
void LoginDialog::on_pushButtonLogin_clicked() {
	if (!getInput()) {
		return;
	}

	connectServer();
	// 向服务器发送用户名和密码，并将响应码用信号传递
	// ------To Do: 加密密码-------
	message::list l;
	l.push(username.toStdString());
	l.push(password.toStdString());

	cli->socket()->emit("sign in", l, [&](const message::list l) {
		Q_EMIT requestSignIn(l[0]->get_int());
	});
}

// 点击注册按钮响应事件
void LoginDialog::on_pushButtonRegister_clicked() {
	if (!getInput()) {
		return;
	}

	connectServer();
	message::list l;
	l.push(username.toStdString());
	l.push(password.toStdString());

	cli->socket()->emit("sign up", l, [&](const message::list l) {
		Q_EMIT requestSignUp(l[0]->get_int());
	});
}

void LoginDialog::signin(int type) {
	MainWindow* w = nullptr;
	switch (type) {
		case 0:
			cli->close();
			this->close();
			w = new MainWindow(username);
			w->show();
			break;
		case 1:
			QMessageBox::warning(this, "warning",
								 QString("User has signed in!"));
			break;
		case 2:
			QMessageBox::warning(
				this, "warning",
				QString("Fail to login, invalid username/password!"));
			break;
		default:
			QMessageBox::warning(this, "warning", QString("Fail to sign in!"));
			break;
	}
}

void LoginDialog::signup(int type) {
	switch (type) {
		case 0:
			if (QMessageBox::StandardButton::Yes ==
				QMessageBox::question(
					this, "information",
					QString("Successfully Registered! Sign in right now?"))) {
				Q_EMIT requestSignIn(0);
			}
			break;
		case 1:
			QMessageBox::warning(
				this, "warning",
				QString("Fail to sign up since user existed!"));
			break;
		default:
			QMessageBox::warning(this, "warning", QString("Fail to sign up!"));
			break;
	}
}
