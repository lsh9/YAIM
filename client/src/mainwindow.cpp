#include "mainwindow.h"
#include "file.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSqlError>
#include <QStringListModel>

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

MainWindow::MainWindow(QString username, QWidget* parent)
	: QMainWindow(parent),
	  ui(new Ui::MainWindow),
	  m_sio(new client),
	  m_sock(m_sio->socket()),
	  m_username(username),
	  m_db(File::getUserDatabase(m_username)) {
	ui->setupUi(this);
	init();

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

	// // 登录
	// m_sock->emit("login", m_username.toStdString());

	// // 初始化好友列表
	// initFriendList();

	// // 初始化聊天记录
	// initChatRecord();

	// // 初始化聊天窗口
	// initChatWindow();

	// // 初始化添加好友窗口
	// initAddFriendWindow();

	// // 初始化删除好友窗口
	// initRemoveFriendWindow();

	// // 初始化错误窗口
	// initErrorWindow();

	//    m_sio->set_reconnect_listener([&](unsigned t1, unsigned t2){
	//        qDebug() << t1 << " " << t2;
	//    });
	//    m_sio->set_socket_open_listener([&](std::string const& nsp){
	//        qDebug() << QString::fromStdString(nsp);
	//    });
	    m_sio->set_open_listener([&](){
	        m_sock->emit("online", m_username.toStdString());
	    });
	//    m_sio->set_close_listener([&](client::close_reason const& reason){
	//        m_sock->emit("offline", m_username.toStdString());
	//    });
	BIND_EVENT(m_sock, "receive message", std::bind(&MainWindow::onReceiveMessage, this, _1, _2, _3, _4));
	BIND_EVENT(m_sock, "receive friend", std::bind(&MainWindow::onReceiveFriend, this, _1, _2, _3, _4));
	BIND_EVENT(m_sock, "receive ", std::bind(&MainWindow::onReceiveMessage, this, _1, _2, _3, _4));

	connect(this, &MainWindow::requestShowMessage, &MainWindow::showMessage);
}

MainWindow::~MainWindow() {
	delete ui;
}

// void MainWindow::init() {
// 	// 设置窗口标题
// 	setWindowTitle("Chat Client");

// 	// 设置窗口大小
// 	setFixedSize(800, 600);

// 	// 设置窗口图标
// 	setWindowIcon(QIcon(":/images/icon.png"));

// 	// 设置窗口背景
// 	QPalette palette;
// 	palette.setBrush(QPalette::Background,
// 					 QBrush(QPixmap(":/images/background.png")));
// 	setPalette(palette);

// 	// 设置好友列表
// 	ui->friendList->setViewMode(QListView::IconMode);
// 	ui->friendList->setIconSize(QSize(60, 60));
// 	ui->friendList->setResizeMode(QListView::Adjust);
// 	ui->friendList->setSpacing(10);
// 	ui->friendList->setStyleSheet(
// 		"QListWidget::item {"
// 		"border-radius: 5px;"
// 		"border: 1px solid #000000;"
// 		"}"
// 		"QListWidget::item:selected {"
// 		"border: 1px solid #000000;"
// 		"background-color: #000000;"
// 		"}");
// }

// void MainWindow::initFriendList() {
// 	// 从数据库中读取好友列表
// 	QStringList friends = m_db.getFriends();

// 	// 添加好友到好友列表
// 	for (int i = 0; i < friends.size(); ++i) {
// 		QListWidgetItem* item =
// 			new QListWidgetItem(QIcon(":/images/friend.png"), friends[i]);
// 		ui->friendList->addItem(item);
// 	}
// }

// void MainWindow::initChatRecord() {
// 	// 从数据库中读取聊天记录
// 	QStringList records = m_db.getChatRecord();

// 	// 添加聊天记录到聊天记录列表
// 	for (int i = 0; i < records.size(); ++i) {
// 		ui->chatRecord->append(records[i]);
// 	}
// }

// void MainWindow::initChatWindow() {
// 	// 设置聊天窗口标题
// 	m_chatWindow.setWindowTitle("Chat");

// 	// 设置聊天窗口大小
// 	m_chatWindow.setFixedSize(400, 300);

// 	// 设置聊天窗口图标
// 	m_chatWindow.setWindowIcon(QIcon(":/images/icon.png"));

// 	// 设置聊天窗口背景
// 	QPalette palette;
// 	palette.setBrush(QPalette::Background,
// 					 QBrush(QPixmap(":/images/background.png")));
// 	m_chatWindow.setPalette(palette);

// 	// 设置聊天窗口聊天记录
// 	m_chatWindow.ui->chatRecord->setReadOnly(true);
// 	m_chatWindow.ui->chatRecord->setStyleSheet(
// 		"QTextEdit {"
// 		"border: 1px solid #000000;"
// 		"border-radius: 5px;"
// 		"background-color: #FFFFFF;"
// 		"}");
// }

// void MainWindow::initAddFriendWindow() {
// 	// 设置添加好友窗口标题
// 	m_addFriendWindow.setWindowTitle("Add Friend");

// 	// 设置添加好友窗口大小
// 	m_addFriendWindow.setFixedSize(400, 300);

// 	// 设置添加好友窗口图标
// 	m_addFriendWindow.setWindowIcon(QIcon(":/images/icon.png"));

// 	// 设置添加好友窗口背景
// 	QPalette palette;
// 	palette.setBrush(QPalette::Background,
// 					 QBrush(QPixmap(":/images/background.png")));
// 	m_addFriendWindow.setPalette(palette);
// }

// void MainWindow::initErrorWindow() {
// 	// 设置错误窗口标题
// 	m_errorWindow.setWindowTitle("Error");

// 	// 设置错误窗口大小
// 	m_errorWindow.setFixedSize(400, 300);

// 	// 设置错误窗口图标
// 	m_errorWindow.setWindowIcon(QIcon(":/images/icon.png"));

// 	// 设置错误窗口背景
// 	QPalette palette;
// 	palette.setBrush(QPalette::Background,
// 					 QBrush(QPixmap(":/images/background.png")));
// 	m_errorWindow.setPalette(palette);
// }

// void MainWindow::initSuccessWindow() {
// 	// 设置成功窗口标题
// 	m_successWindow.setWindowTitle("Success");

// 	// 设置成功窗口大小
// 	m_successWindow.setFixedSize(400, 300);

// 	// 设置成功窗口图标
// 	m_successWindow.setWindowIcon(QIcon(":/images/icon.png"));

// 	// 设置成功窗口背景
// 	QPalette palette;
// 	palette.setBrush(QPalette::Background,
// 					 QBrush(QPixmap(":/images/background.png")));
// 	m_successWindow.setPalette(palette);
// }

// void MainWindow::initSocket() {
// 	// 设置服务器地址
// 	m_socket.connectToHost(QHostAddress::LocalHost, 8888);

// 	// 连接服务器成功
// 	connect(&m_socket, &QTcpSocket::connected, this, &MainWindow::connected);

// 	// 接收服务器消息
// 	connect(&m_socket, &QTcpSocket::readyRead, this,
// 			&MainWindow::receiveMessage);
// }

// void MainWindow::connected() {
// 	// 发送登录消息
// 	sendMessage(MessageType::Login, m_username);
// }

// void MainWindow::receiveMessage() {
// 	// 读取消息
// 	QDataStream in(&m_socket);
// 	in.setVersion(QDataStream::Qt_5_12);

// 	// 如果消息长度为0，说明是刚刚连接
// 	if (m_messageLength == 0) {
// 		// 如果可用的字节数小于消息长度所占的字节数，说明消息长度还没接收完
// 		if (m_socket.bytesAvailable() < (int)sizeof(quint16)) {
// 			return;
// 		}

// 		// 接收消息长度
// 		in >> m_messageLength;
// 	}

// 	// 如果可用的字节数小于消息长度，说明消息还没接收完
// 	if (m_socket.bytesAvailable() < m_messageLength) {
// 		return;
// 	}

// 	// 接收消息类型
// 	MessageType type;
// 	in >> type;

// 	// 接收消息
// 	QString message;
// 	in >> message;

// 	// 处理消息
// 	handleMessage(type, message);

// 	// 重置消息长度
// 	m_messageLength = 0;
// }

// void MainWindow::handleMessage(MessageType type, QString message) {
// 	switch (type) {
// 		case MessageType::Login:
// 			handleLogin(message);
// 			break;
// 		case MessageType::Logout:
// 			handleLogout(message);
// 			break;
// 		case MessageType::AddFriend:
// 			handleAddFriend(message);
// 			break;
// 		case MessageType::Chat:
// 			handleChat(message);
// 			break;
// 		case MessageType::Error:
// 			handleError(message);
// 			break;
// 		case MessageType::Success:
// 			handleSuccess(message);
// 			break;
// 	}
// }

// void MainWindow::handleLogin(QString message) {
// 	// 如果是自己登录，显示聊天窗口
// 	if (message == m_username) {
// 		m_chatWindow.show();
// 		return;
// 	}

// 	// 如果是别人登录，添加好友
// 	m_chatWindow.ui->friendList->addItem(message);
// }

// void MainWindow::handleLogout(QString message) {
// 	// 如果是自己退出，退出程序
// 	if (message == m_username) {
// 		exit(0);
// 	}

// 	// 如果是别人退出，删除好友
// 	for (int i = 0; i < m_chatWindow.ui->friendList->count(); i++) {
// 		if (m_chatWindow.ui->friendList->item(i)->text() == message) {
// 			m_chatWindow.ui->friendList->takeItem(i);
// 			break;
// 		}
// 	}
// }

// void MainWindow::handleAddFriend(QString message) {
// 	// 如果是自己添加好友，显示成功窗口
// 	if (message == m_username) {
// 		m_successWindow.show();
// 		return;
// 	}

// 	// 如果是别人添加好友，添加好友
// 	m_chatWindow.ui->friendList->addItem(message);
// }

// void MainWindow::handleChat(QString message) {
// 	// 解析消息
// 	QStringList list = message.split(":");

// 	// 如果是自己发的消息，显示在聊天窗口
// 	if (list[0] == m_username) {
// 		m_chatWindow.ui->chatBrowser->append("我：" + list[1]);
// 		return;
// 	}

// 	// 如果是别人发的消息，显示在聊天窗口
// 	m_chatWindow.ui->chatBrowser->append(list[0] + "：" + list[1]);
// }

// void MainWindow::handleError(QString message) {
// 	// 显示错误窗口
// 	m_errorWindow.ui->errorLabel->setText(message);
// 	m_errorWindow.show();
// }

// void MainWindow::handleSuccess(QString message) {
// 	// 显示成功窗口
// 	m_successWindow.ui->successLabel->setText(message);
// 	m_successWindow.show();
// }

// void MainWindow::sendMessage(MessageType type, QString message) {
// 	// 构造消息
// 	QByteArray data;
// 	QDataStream out(&data, QIODevice::WriteOnly);
// 	out.setVersion(QDataStream::Qt_5_12);
// 	out << (quint16)0;
// 	out << type;
// 	out << message;
// 	out.device()->seek(0);
// 	out << (quint16)(data.size() - sizeof(quint16));

// 	// 发送消息
// 	m_socket.write(data);
// }

// void MainWindow::on_loginButton_clicked() {
// 	// 获取用户名
// 	m_username = m_loginWindow.ui->usernameEdit->text();

// 	// 发送登录消息
// 	sendMessage(MessageType::Login, m_username);
// }

// void MainWindow::on_logoutButton_clicked() {
// 	// 发送退出消息
// 	sendMessage(MessageType::Logout, m_username);
// }

// void MainWindow::on_addFriendButton_clicked() {
// 	// 获取好友名
// 	QString friendName = m_chatWindow.ui->friendEdit->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_sendButton_clicked() {
// 	// 获取聊天内容
// 	QString message = m_chatWindow.ui->chatEdit->toPlainText();

// 	// 获取好友名
// 	QString friendName = m_chatWindow.ui->friendList->currentItem()->text();

// 	// 发送聊天消息
// 	sendMessage(MessageType::Chat, friendName + ":" + message);

// 	// 清空聊天内容
// 	m_chatWindow.ui->chatEdit->clear();
// }

// void MainWindow::on_errorButton_clicked() {
// 	// 隐藏错误窗口
// 	m_errorWindow.hide();
// }

// void MainWindow::on_successButton_clicked() {
// 	// 隐藏成功窗口
// 	m_successWindow.hide();
// }

// void MainWindow::on_friendList_itemDoubleClicked(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemClicked(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemActivated(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemSelectionChanged() {
// 	// 获取好友名
// 	QString friendName = m_chatWindow.ui->friendList->currentItem()->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemEntered(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemChanged(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemPressed(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemPressed(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemPressed(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemPressed(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

// void MainWindow::on_friendList_itemPressed(QListWidgetItem* item) {
// 	// 获取好友名
// 	QString friendName = item->text();

// 	// 发送添加好友消息
// 	sendMessage(MessageType::AddFriend, friendName);
// }

void MainWindow::closeEvent(QCloseEvent* event) {
	m_sio->close();
}

void MainWindow::init() {
	ui->label->setText(m_username);
	ui->labelReceiver->setText(m_username);
	showFriendList();
}

void MainWindow::initMessageList() {
	//	m_sock->emit("get messages list", m_username.toStdString(), [&](sio::message::list const& msg) {
	//		sio::message::list const& list = msg[0]->get_vector();
	//		for (auto const& item : list) {
	//			sio::message::list const& message = item->get_map();
	//			QString sender = QString::fromStdString(message[0]->get_string());
	//			QString receiver = QString::fromStdString(message[1]->get_string());
	//			QString content = QString::fromStdString(message[2]->get_string());
	//			QString time = QString::fromStdString(message[3]->get_string());
	//			QString type = QString::fromStdString(message[4]->get_string());
	//			if (type == "chat") {
	//				if (sender == m_username) {
	//					m_chatWindow.ui->chatBrowser->append("我 " + time);
	//					m_chatWindow.ui->chatBrowser->append(content);
	//				} else {
	//					m_chatWindow.ui->chatBrowser->append(sender + " " + time);
	//					m_chatWindow.ui->chatBrowser->append(content);
	//				}
	//			} else if (type == "add friend") {
	//				if (sender == m_username) {
	//					m_chatWindow.ui->chatBrowser->append("我 " + time);
	//					m_chatWindow.ui->chatBrowser->append("添加好友 " + receiver);
	//				} else {
	//					m_chatWindow.ui->chatBrowser->append(sender + " " + time);
	//					m_chatWindow.ui->chatBrowser->append("添加好友 " + sender);
	//				}
	//			}
	//		}
	//	});
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

void MainWindow::showFriendList() {
	// --------------to do: read local cache-------------
	m_sock->emit("get friends list", m_username.toStdString(), [&](const message::list l) {
		auto array = l[0]->get_vector();
		auto size = array.size();
		for (size_t i = 0; i < size; i++) {
			auto& map = array[i]->get_map();
			auto& str = map["friend"]->get_string();
			m_db.exec("insert into friend values('" + QString::fromStdString(str) + "')");
			ui->listWidget->addItem(str.c_str());
		}
	});
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
	QString msg = ui->textEdit->toPlainText();
	if (!msg.isEmpty()) {
		// 打包消息
		std::string sender = m_username.toStdString();
		std::string receiver = ui->labelReceiver->text().toStdString();
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
		ui->textEdit->clear();
	}
}

void MainWindow::on_listWidget_itemSelectionChanged() {
	// get friend's username
	ui->stackedWidget->setCurrentWidget(ui->pageMessage);
	auto item = ui->listWidget->currentItem();
	ui->labelReceiver->setText(item->text());
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

	auto& message_list = this->m_message_list[item->text().toStdString()];
	for (auto& message : message_list) {
		auto& sender = message.second["sender"]->get_string();
		auto& receiver = message.second["receiver"]->get_string();
		auto& content = message.second["content"]->get_string();
		auto& time = message.second["time"]->get_string();
        QString label = QString::fromStdString(sender + " : " + content);
		ui->textBrowser->append(label);
	}
}
