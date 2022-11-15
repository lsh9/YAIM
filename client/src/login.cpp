#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include "socket.h"
#include <QMessageBox>

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

bool Login::getInput()
{
    username = ui->lineEditUsername->text();
    password = ui->lineEditPassword->text();
    if (username.isEmpty() || username.size() < 6){
        QMessageBox::warning(this, "warning", QString("Please input valid username!"));
        return false;
    }
    if (password.isEmpty() || password.size() < 6){
        QMessageBox::warning(this, "warning", QString("Please input valid password!"));
        return false;
    }
    return true;
}


void Login::on_pushButtonLogin_clicked()
{
    if(!getInput()){
        return;
    }

    socket->connectToHost(IP, PORT);
    socket->write(username.toStdString().c_str(), 16);
    socket->write(password.toStdString().c_str(), 16);
    // 登录成功
    this->close();
    MainWindow* m = new MainWindow(username, this);
    m->show();
}


void Login::on_pushButtonRegister_clicked()
{
    if(!getInput()){
        return;
    }

    QTcpSocket *tmp_sock = new QTcpSocket();
    tmp_sock->connectToHost(IP, PORT);
    tmp_sock->write(username.toStdString().c_str(), username.length());
    char  code = -1;
    tmp_sock->waitForReadyRead(3000);

    if (tmp_sock->read(&code, 1)  <= 0){
        QMessageBox::information(this, "information", QString("Out of time, please try again!"));
    }
    else if (code == 0x0){
        // successfully register
        QMessageBox::information(this, "information", QString("Successfully registered!"));
    }
    else if (code == 0x1){
        // user already exists
        QMessageBox::warning(this, "warning", QString("Fail to register since user existed!"));
    }
    tmp_sock->close();
}

