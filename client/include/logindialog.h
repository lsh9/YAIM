#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QString>
#include <QThread>

#include "sio_client.h"

using namespace sio;


QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();



Q_SIGNALS:
    void requestSignIn(int);
    void requestSignUp(int);

private Q_SLOTS:
    void on_pushButtonLogin_clicked();
    void on_pushButtonRegister_clicked();

private:
    bool getInput();
    void connectServer();
    void signin(int);
    void signup(int);

private:
    Ui::Login *ui;
    QString username;
    QString password;
    std::unique_ptr<client> cli;

};

#endif // LOGINDIALOG_H
