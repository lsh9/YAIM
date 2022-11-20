#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

#include "sio_client.h"
using namespace sio;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef std::map<std::string, message::ptr> Message;

class MainWindow : public QMainWindow {
	Q_OBJECT

   public:
	MainWindow(QString username, QWidget* parent = nullptr);
	~MainWindow();
	void closeEvent(QCloseEvent* event);

   private:
	// manage content of widgets in window
	void init();  // read/get user info before show window
	void initDatabase();
	void initFriendList();
	void initMessageList();
	void initFriendRequestList();
	void showMessage(const std::string sender,
					 const std::string receiver,
					 const std::string content,
					 const std::string time);
	void showFriendList();

   Q_SIGNALS:
	void requestShowFriendList();
	void requestShowMessage(const std::string sender,
							const std::string receiver,
							const std::string content,
							const std::string time);

   private Q_SLOTS:
	// slots for widgets, emit events
	void on_pushButton_clicked();
	void on_listWidget_itemSelectionChanged();

   private:
	// event handler
	void onReceiveMessage(std::string const& name, message::ptr const& data, bool hasAck, message::list& ack_resp);
	void onReceiveFriend(std::string const& name, message::ptr const& data, bool hasAck, message::list& ack_resp);

   private:
	Ui::MainWindow* ui;
	std::unique_ptr<client> m_sio;
	socket::ptr m_sock;
	QString m_username;
	QSqlDatabase m_db;
	std::map<std::string, std::string> m_friend_list;
	std::map<std::string, std::map<int, Message>> m_message_list;
};
#endif	// MAINWINDOW_H
