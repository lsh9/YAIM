#include "logindialog.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDialog d[3];
    d[0].show();    d[1].show(); d[2].show();
    return a.exec();
}
