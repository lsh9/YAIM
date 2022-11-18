#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QStringListModel>
#include <QListWidgetItem>

#define kURL "ws://localhost:3000"
#ifdef WIN32
#define BIND_EVENT(IO,EV,FN) \
do{ \
        socket::event_listener_aux l = FN;\
        IO->on(EV,l);\
} while(0)

#else
#define BIND_EVENT(IO,EV,FN) \
IO->on(EV,FN)
#endif


    MainWindow::MainWindow(QString username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_sio(new client)
    , m_sock(m_sio->socket())
    , m_username(username)
{
    ui->setupUi(this);
    ui->label->setText(m_username);
    init();

    m_sio->connect(kURL);
    m_sock->emit("online", m_username.toStdString());
    // 绑定监听的事件
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;
    BIND_EVENT(m_sock,"receive", std::bind(&MainWindow::onReceive, this, _1, _2, _3, _4));
}

MainWindow::~MainWindow()
{
    m_sock->emit("offline", m_username.toStdString());
    m_sio->close();
    delete ui;
}


void MainWindow::init()
{
    m_sock->emit("get friends list", m_username.toStdString(), [&](const message::list l){
        QStringList list;
        auto array = l[0]->get_vector();
        auto size = array.size();
        for(size_t i=0; i < size; i++){
            auto& map = array[i]->get_map();
            auto& str = map["friend"]->get_string();
            list.append(str.c_str());
        }
        QStringListModel *list_model = new QStringListModel(list);
        ui->listView->setModel(list_model);
    });
}

void MainWindow::onReceive(std::string const& name, message::ptr const& data, bool hasAck, message::list &ack_resp)
{
    qDebug() << QString::fromStdString(name);
    if(data->get_flag() == message::flag_object)
    {
        auto obj = data->get_map();
        std::string content = obj["content"]->get_string();
        std::string sender = obj["sender"]->get_string();
        std::string receiver = obj["receiver"]->get_string();
        qDebug() << QString::fromStdString(sender + ": " + content + " to: " + receiver);
        QString label = QString::fromUtf8(sender.data(),sender.length());
        label.append(" : ");
        label.append(QString::fromUtf8(content.data(),content.length()));
        ui->textBrowser->append(label);
    }
}


void MainWindow::on_pushButton_clicked()
{
    QString msg = ui->textEdit->toPlainText();
    if (!msg.isEmpty()){

        auto ptr = object_message::create();
        object_message* x = dynamic_cast<object_message*>(ptr.get());
        x->insert("sender", m_username.toStdString());
        x->insert("content", msg.toStdString());
        x->insert("receiver", "111111");

        m_sock->emit("send", ptr);
    }
    ui->textEdit->clear();
}

