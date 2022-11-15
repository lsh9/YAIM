QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/login.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/socket.cpp

HEADERS += \
    ./include/login.h \
	./include/mainwindow.h \
	./include/socket.h


FORMS += \
    ui/login.ui \
    ui/mainwindow.ui



INCLUDEPATH += include/
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	yaim.qrc
