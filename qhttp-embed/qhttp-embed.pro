#-------------------------------------------------
#
# Project created by QtCreator 2018-09-12T16:19:20
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET    = qhttp-embed
TEMPLATE  = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PRJDIR    = .
include($$PRJDIR/commondir.pri)

DEFINES       *= QHTTP_MEMORY_LOG=0
win32:DEFINES *= QHTTP_EXPORT

# Joyent http_parser
SOURCES  += $$PRJDIR/3rdparty/http-parser/http_parser.c
HEADERS  += $$PRJDIR/3rdparty/http-parser/http_parser.h

SOURCES  += \
    qhttpabstracts.cpp \
    qhttpserverconnection.cpp \
    qhttpserverrequest.cpp \
    qhttpserverresponse.cpp \
    qhttpserver.cpp

HEADERS  += \
    qhttpfwd.hpp \
    qhttpabstracts.hpp \
    qhttpserverconnection.hpp \
    qhttpserverrequest.hpp \
    qhttpserverresponse.hpp \
    qhttpserver.hpp

contains(DEFINES, QHTTP_HAS_CLIENT) {
    SOURCES += \
        qhttpclientrequest.cpp \
        qhttpclientresponse.cpp \
        qhttpclient.cpp

    HEADERS += \
        qhttpclient.hpp \
        qhttpclientresponse.hpp \
        qhttpclientrequest.hpp
}

DISTFILES += \
    README.md \
    LICENSE
