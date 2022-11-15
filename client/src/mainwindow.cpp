#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socket.h"

MainWindow::MainWindow(QString username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), username(username)
{
    ui->setupUi(this);
    ui->label->setText(username);
}

MainWindow::~MainWindow()
{
    socket->close();
    delete ui;
}

