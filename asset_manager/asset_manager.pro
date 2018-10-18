#-------------------------------------------------
#
# Project created by QtCreator 2018-09-13T22:33:55
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = asset_manager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

QMAKE_LIBS += -static

SOURCES += \
        app/main.cpp \
        mainwindow.cpp \
    network/message.cpp \
    network/requestmessage.cpp \
    network/responsemessage.cpp

HEADERS += \
        mainwindow.h \
    network/message.h \
    network/requestmessage.h \
    network/responsemessage.h

FORMS += \
        mainwindow.ui

BINDIR = $$OUT_PWD/../xbin
DESTDIR = $$BINDIR
LIBS += -L$$BINDIR -lqhttp-embed

INCLUDEPATH += $$PWD/../qhttp-embed
DEPENDPATH += $$PWD/../qhttp-embed

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
