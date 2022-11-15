#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socket.h"

MainWindow::MainWindow(QString username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), username(username)
{
    ui->setupUi(this);
    this->setWindowTitle(username);
}

MainWindow::~MainWindow()
{
    socket->close();
    delete ui;
}

