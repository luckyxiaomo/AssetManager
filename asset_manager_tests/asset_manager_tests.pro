QT += testlib
QT += core gui widgets network
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp \
    tst_testexample.cpp \

HEADERS += autotest.h \
    tst_testexample.h \


MAIN_PROJ=$$PWD/../asset_manager

# Files incorporated from the main asset_manager project.
# After adding new files, if you get an undefined reference error,
# be sure to execute 'qmake' on the test project folder.
SOURCES += \
        $$MAIN_PROJ/*.cpp \
        $$MAIN_PROJ/network/*.cpp

HEADERS += \
        $$MAIN_PROJ/*.h \
        $$MAIN_PROJ/network/*.h

FORMS += \
        $$MAIN_PROJ/*.ui
# End of section from main asset_manager project

BINDIR = $$OUT_PWD/../xbin
DESTDIR = $$BINDIR
LIBS += -L$$BINDIR -lqhttp-embed

INCLUDEPATH += $$MAIN_PROJ
INCLUDEPATH += $$PWD/../qhttp-embed
DEPENDPATH += $$PWD/../qhttp-embed
