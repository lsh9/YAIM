#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sio_client.h"
using namespace sio;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString username, QWidget *parent = nullptr);
    ~MainWindow();


Q_SIGNALS:


private Q_SLOTS:

    void on_pushButton_clicked();

private:
    // private functions
    void init();  // read/get user info before show window
    void onReceive(std::string const& name,message::ptr const& data,bool hasAck,message::list &ack_resp);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<client> m_sio;
    socket::ptr m_sock;
    QString m_username;
};
#endif // MAINWINDOW_H
