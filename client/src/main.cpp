#include "logindialog.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDialog d;
    d.show();
    LoginDialog d1;
    d1.show();
    return a.exec();
}
