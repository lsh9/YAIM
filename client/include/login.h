#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QString>


QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QDialog
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private:
    bool getInput();

private slots:
    void on_pushButtonLogin_clicked();
    void on_pushButtonRegister_clicked();


private:
    Ui::Login *ui;
    QString username;
    QString password;

};

#endif // LOGIN_H
