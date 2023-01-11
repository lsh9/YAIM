QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32-msvc*: {
    QMAKE_CFLAGS *= /utf-8
    QMAKE_CXXFLAGS *= /utf-8
}
CONFIG += c++11
CONFIG += no_keywords
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/database.cpp \
    src/file.cpp \
    src/logindialog.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/request.cpp

HEADERS += \
    include/database.h \
    include/defines.h \
    include/file.h \
    include/logindialog.h \
	include/mainwindow.h \
    include/request.h

FORMS += \
    ui/logindialog.ui \
    ui/mainwindow.ui \
	ui/friendsPages.ui \
	ui/createGroup.ui

INCLUDEPATH += include/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	resource/yaim.qrc

RC_ICONS = resource/yaim16x16.ico

INCLUDEPATH += $$PWD/socket-io/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/socket-io/lib/release/ -lsioclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/socket-io/lib/debug/ -lsioclient



