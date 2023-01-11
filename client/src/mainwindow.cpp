#include "mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSqlError>
#include <QStringListModel>
#include "defines.h"
#include "file.h"
#include "ui_mainwindow.h"

#ifdef WIN32
#define BIND_EVENT(IO, EV, FN)             \
	do {                                   \
		socket::event_listener_aux l = FN; \
		IO->on(EV, l);                     \
	} while (0)

#else
#define BIND_EVENT(IO, EV, FN) IO->on(EV, FN)
#endif

MainWindow::MainWindow(int user_id, QWidget* parent)
	: QMainWindow(parent),
	  ui(new Ui::MainWindow),
	  m_sio(new client),
	  m_sock(m_sio->socket()),
	  m_userid_ptr(object_message::create()),
	  m_user_id(user_id)
// m_db(File::getUserDatabase(" "))
{
	ui->setupUi(this);

	connect(this, &MainWindow::getUserInfo, this, &MainWindow::showUserInfo, Qt::BlockingQueuedConnection);
    connect(this, &MainWindow::getFriendInfo, this, &MainWindow::showFriendInfo, Qt::BlockingQueuedConnection);
    connect(this, &MainWindow::requestShowMessage, &MainWindow::showMessage);

	// 由于user_id多次发送给服务器，直接打包好方便通信
	m_userid_ptr->get_map()["user_id"] = int_message::create(m_user_id);

	// 监听连接状况
	m_sio->set_reconnect_listener([&](unsigned t1, unsigned t2) { qDebug() << t1 << " " << t2; });
	m_sio->set_socket_open_listener([&](std::string const& nsp) { qDebug() << QString::fromStdString(nsp); });
	m_sio->set_open_listener([&]() { m_sock->emit("online", m_userid_ptr, [&](const message::list l) {}); });
	m_sio->set_close_listener([&](client::close_reason const& reason) { m_sock->emit("offline", m_userid_ptr); });
	m_sio->connect(kURL);

	// 绑定监听的事件
	using std::placeholders::_1;
	using std::placeholders::_2;
	using std::placeholders::_3;
	using std::placeholders::_4;
	//	BIND_EVENT(m_sock, "connect", std::bind(&MainWindow::onConnect, this, _1));
	//	BIND_EVENT(m_sock, "disconnect", std::bind(&MainWindow::onDisconnect, this, _1));
	//	BIND_EVENT(m_sock, "login", std::bind(&MainWindow::onLogin, this, _1, _2, _3, _4));
	//	BIND_EVENT(m_sock, "logout", std::bind(&MainWindow::onLogout, this, _1, _2, _3, _4));
	//	BIND_EVENT(m_sock, "message", std::bind(&MainWindow::onMessage, this, _1, _2, _3, _4));
	//	BIND_EVENT(m_sock, "addUser", std::bind(&MainWindow::onAddUser, this, _1, _2, _3, _4));
	//	BIND_EVENT(m_sock, "addFriend", std::bind(&MainWindow::onAddFriend, this, _1, _2, _3, _4));
	//	BIND_EVENT(m_sock, "removeFriend", std::bind(&MainWindow::onRemoveFriend, this, _1, _2, _3, _4));
	//	BIND_EVENT(m_sock, "error", std::bind(&MainWindow::onError, this, _1, _2, _3, _4));

	BIND_EVENT(m_sock, "receive message", std::bind(&MainWindow::onReceiveMessage, this, _1, _2, _3, _4));
	BIND_EVENT(m_sock, "receive friend", std::bind(&MainWindow::onReceiveFriend, this, _1, _2, _3, _4));
	BIND_EVENT(m_sock, "receive ", std::bind(&MainWindow::onReceiveMessage, this, _1, _2, _3, _4));

	init();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
	m_sio->close();
}

void MainWindow::init() {
	m_sock->emit("get user information", m_userid_ptr,
				 [&](const message::list l) { Q_EMIT getUserInfo(l[0]->get_map()); });

	m_sock->emit("get friend list", m_userid_ptr, [&](const message::list l) {
		for (auto& item : l.to_array_message()->get_vector()) {
			auto ptr = object_message::create();
			ptr->get_map()["user_id"] = item->get_map()["user_id"];
			m_sock->emit("get user information", ptr,
						 [&](const message::list l) { Q_EMIT getFriendInfo(l[0]->get_map()); });
		}
		Q_EMIT getFriendList();
	});

    m_sock->emit("get group list", m_userid_ptr, [&](const message::list l) {
        for (auto& item : l.to_array_message()->get_vector()) {
            auto ptr = object_message::create();
            ptr->get_map()["user_id"] = item->get_map()["user_id"];
            m_sock->emit("get user information", ptr,
                         [&](const message::list l) { Q_EMIT getFriendInfo(l[0]->get_map()); });
        }
        Q_EMIT getFriendList();
    });

	//	showFriendList();

	//    initUnreadMessages();
}

void MainWindow::initMessageList() {}

void MainWindow::initUnreadMessages() {
	m_sock->emit("get unread messages", int_message::create(m_user_id), [=](sio::message::list const& msg) {
		if (msg.size() == 0) {
			return;
		}
		auto const& data = msg[0]->get_vector();
		for (auto& item : data) {
			auto message = item->get_map();
			auto sender = message["sender"]->get_string();
			auto content = message["content"]->get_string();
			auto time = message["time"]->get_string();
			auto receiver = message["receiver"]->get_string();
			int msgid = message["msgid"]->get_int();
			// QString type = QString::fromStdString(message.at("type")->get_string());
			this->m_message_list[sender][msgid] = message;
		}
	});
}

void MainWindow::showMessage(const std::string sender,
							 const std::string receiver,
							 const std::string content,
							 const std::string time) {
	qDebug() << QString::fromStdString(sender + ": " + content + " to: " + receiver);
	QString label = QString::fromUtf8(sender.data(), sender.length());
	label.append(" : ");
	label.append(QString::fromUtf8(content.data(), content.length()));
	ui->textBrowser->append(label);
}

void MainWindow::onReceiveMessage(std::string const& name,
								  message::ptr const& data,
								  bool hasAck,
								  message::list& ack_resp) {
	qDebug() << QString::fromStdString(name);
	if (data->get_flag() == message::flag_object) {
		auto obj = data->get_map();
		auto msgid = obj["msgid"]->get_int();
		std::string sender = obj["sender"]->get_string();
		std::string receiver = obj["receiver"]->get_string();
		std::string content = obj["content"]->get_string();
		std::string time = obj["time"]->get_string();
		//		QSqlQuery query(m_db);
		//		if (!query.exec(QString("insert into message values('%1', '%2', '%3', '%4')")
		//							.arg(sender.c_str())
		//							.arg(receiver.c_str())
		//							.arg(content.c_str())
		//							.arg(time.c_str()))) {
		//			qDebug() << "Fail to insert!";
		//		}
		this->m_message_list[sender][msgid] = obj;
		Q_EMIT requestShowMessage(sender, receiver, content, time);
	}
}

void MainWindow::onReceiveFriend(const std::string& name,
								 const message::ptr& data,
								 bool hasAck,
								 message::list& ack_resp) {
	if (data->get_flag() == message::flag_string) {
		auto requester = data->get_string();
		qDebug() << QString::fromStdString(requester + " request to make friend with you.");
	}
}

void MainWindow::on_pushButton_clicked() {
	QString msg = ui->textEdit_Input->toPlainText();
	if (!msg.isEmpty()) {
		// 打包消息
		std::string sender = m_username;
		std::string receiver = ui->label_Friendname->text().toStdString();
		std::string content = msg.toStdString();
		auto ptr = object_message::create();
		ptr->get_map()[std::string("sender")] = string_message::create(sender);
		ptr->get_map()[std::string("receiver")] = string_message::create(receiver);
		ptr->get_map()[std::string("content")] = string_message::create(content);

		// 发送消息，接收到时间后将消息存入数据库
		m_sock->emit("send message", ptr, [=](const message::list l) {
			auto& result = l[0]->get_map();
			auto& time = result["time"]->get_string();
			auto msgid = result["msgid"]->get_int();
			//			QSqlQuery query(m_db);
			//			if (!query.exec(
			//					QString("insert into message sender, receiver, content, time values('%1', '%2', '%3',
			//'%4')") 						.arg(sender.c_str()) 						.arg(receiver.c_str())
			//.arg(content.c_str()) 						.arg(time.c_str()))) { 				qDebug()
			//<< "Fail to insert!";
			//			}
			ptr->get_map()[std::string("time")] = string_message::create(sender);
			this->m_message_list[receiver][msgid] = (ptr->get_map());
			Q_EMIT requestShowMessage(sender, receiver, content, time);
		});
		ui->textEdit_Input->clear();
	}
}

void MainWindow::on_listWidget_itemSelectionChanged() {
	// get friend's username
	ui->stackedWidget->setCurrentWidget(ui->pageMessage);
	auto item = ui->listWidget_GroupwithFriends->currentItem();
	ui->label_Friendname->setText(item->text());
	ui->textBrowser->clear();

	// --------------to do: read local cache-------------

	//	QSqlQuery query(m_db);
	//	query.exec(QString("select * from message where sender='%1' and receiver='%2' or sender='%2' and "
	//					   "receiver='%1'order by time asc")
	//				   .arg(m_username)
	//				   .arg(item->text()));
	//	while (query.next()) {
	//		QString sender = query.value(0).toString();
	//		QString receiver = query.value(1).toString();
	//		QString content = query.value(2).toString();
	//		QString time = query.value(3).toString();
	//		QString label = sender + " : " + content;
	//		ui->textBrowser->append(label);
	//	}

	auto& cur_message_list = this->m_message_list[item->text().toStdString()];
	for (auto& message : cur_message_list) {
		auto& sender = message.second["sender"]->get_string();
		auto& receiver = message.second["receiver"]->get_string();
		auto& content = message.second["content"]->get_string();
		auto& time = message.second["time"]->get_string();
		QString label = QString::fromStdString(sender + " : " + content);
		ui->textBrowser->append(label);
	}
}

void MainWindow::on_textEdit_copyAvailable(bool b) {
	qDebug() << b;
}

void MainWindow::showUserInfo(ResponseObject resp) {

    m_username = resp["username"]->get_string();
    m_nickname = resp["nickname"]->get_string();
    m_avatar_url = resp["avatar"]->get_string();
    ui->lineEditUsername->setText(QString::fromStdString(m_username));
    ui->lineEditNickname->setText(QString::fromStdString(m_nickname));
    // To-do: load avatar

}

void MainWindow::showFriendInfo(ResponseObject resp) {
    int friend_id = resp["user_id"]->get_int();
	m_friend_list[friend_id] = resp;
	// To-do: load avatar
	ui->listWidget_GroupwithFriends->addItem(QString::fromStdString(resp["nickname"]->get_string()));
}
