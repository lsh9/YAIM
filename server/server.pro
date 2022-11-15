QT -= gui sql
QT += network sql

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/client.cpp \
        src/database.cpp \
        src/server.cpp

HEADERS += \
    include/client.h \
    include/database.h \
    include/server.h

INCLUDEPATH += include/
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -llibmysql

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
