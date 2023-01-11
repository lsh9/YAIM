
#include "logindialog.h"
#include "mainwindow.h"
#include "ui_logindialog.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QMetaType>

#ifdef WIN32
#define BIND_EVENT(IO, EV, FN)             \
	do {                                   \
		socket::event_listener_aux l = FN; \
		IO->on(EV, l);                     \
	} while (0)
#else
#define BIND_EVENT(IO, EV, FN) IO->on(EV, FN)
#endif

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::Login), cli(new client()) {
	ui->setupUi(this);
	qRegisterMetaType<ResponseObject>("ResponseObject");
	qRegisterMetaType<ResponseObject>("ResponseObject&");
	connect(this, &LoginDialog::requestLogin, this, &LoginDialog::login, Qt::BlockingQueuedConnection);
	connect(this, &LoginDialog::requestRegister, this, &LoginDialog::registration);
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
		QMessageBox::warning(this, "warning", QString("Please input valid username!"));
		return false;
	}
	if (password.isEmpty() || password.size() < 6) {
		QMessageBox::warning(this, "warning", QString("Please input valid password!"));
		return false;
	}
	return true;
}

void LoginDialog::connectServer() {
	if (!cli->opened()) {
		cli->connect(SERVER);
		qDebug() << "session id: " << QString::fromStdString(cli->get_sessionid());
	}
}

// 点击登录按钮响应事件
void LoginDialog::on_pushButtonLogin_clicked() {
	if (!getInput()) {
		return;
	}

	connectServer();
	// 通信
	auto ptr = object_message::create();
	std::string md5_password =
		QCryptographicHash::hash(password.toStdString().c_str(), QCryptographicHash::Md5).toHex().toStdString();

	ptr->get_map()[std::string("username")] = string_message::create(username.toStdString());
	ptr->get_map()[std::string("password")] = string_message::create(md5_password);
	cli->socket()->emit("login", ptr, [&](const message::list l) { Q_EMIT requestLogin(l[0]->get_map()); });
}

// 点击注册按钮响应事件
void LoginDialog::on_pushButtonRegister_clicked() {
	if (!getInput()) {
		return;
	}

	bool ok = false;
	nickname = QInputDialog::getText(this, tr("注册"), tr("请输入昵称:"), QLineEdit::Normal, "", &ok);
	if (!ok) {
		return;
	}
	if (nickname.isEmpty()) {
		QMessageBox::information(NULL, "warning", "无效昵称！");
		return;
	}

	connectServer();
	// 通信
	auto ptr = object_message::create();
	std::string md5_password =
		QCryptographicHash::hash(password.toStdString().c_str(), QCryptographicHash::Md5).toHex().toStdString();

	ptr->get_map()[std::string("nickname")] = string_message::create(nickname.toStdString());
	ptr->get_map()[std::string("username")] = string_message::create(username.toStdString());
	ptr->get_map()[std::string("password")] = string_message::create(md5_password);

	cli->socket()->emit("register", ptr, [&](const message::list l) { Q_EMIT requestRegister(l[0]->get_map()); });
}

void LoginDialog::login(ResponseObject resp) {
	MainWindow* w = nullptr;
	int code = resp["code"]->get_int();
	switch (code) {
		case NORMAL:
			cli->close();
			this->close();
			w = new MainWindow(resp["user_id"]->get_int());
			w->show();
			break;
		case ACCOUNT_ALREADY_LOGIN_ERROR:
			QMessageBox::warning(this, "warning", QString("用户已登录！"));
			break;
		case ACCOUNT_NOT_EXIST_ERROR:
			QMessageBox::warning(this, "warning", QString("用户不存在！"));
			break;
		case ACCOUNT_PASSWORD_ERROR:
			QMessageBox::warning(this, "warning", QString("密码错误！"));
			break;
		default:
			QMessageBox::warning(this, "warning", QString("服务器异常！登陆失败。"));
			break;
	}
}

void LoginDialog::registration(ResponseObject resp) {
	int code = resp["code"]->get_int();
	switch (code) {
		case NORMAL:
			if (QMessageBox::StandardButton::Yes ==
				QMessageBox::question(this, "information", QString("注册成功！是否立即登录？"))) {
				Q_EMIT requestLogin(resp);
			}
			break;
		case ACCOUNT_EXISTED_ERROR:
			QMessageBox::warning(this, "warning", QString("用户已存在！"));
			break;
		default:
			QMessageBox::warning(this, "warning", QString("服务器异常！注册失败。"));
			break;
	}
}
