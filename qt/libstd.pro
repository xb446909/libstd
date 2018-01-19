#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T22:39:56
#
#-------------------------------------------------

QT       -= core gui

TARGET = libstd
TEMPLATE = lib
#CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    ../src/socket/socket.cpp \
    ../src/iniconfig/iniconfig.cpp \
    ../src/socket/csocket.cpp \
    ../src/socket/ctcpserver.cpp \
    ../src/socket/tcpserverproc.cpp \
    ../src/socket/ctcpclient.cpp

HEADERS += \
    ../inc/socket.h \
    ../inc/iniconfig.h \
    ../src/socket/csocket.h \
    ../src/socket/ctcpserver.h \
    ../src/socket/tcpserverproc.h \
    ../src/socket/ctcpclient.h

INCLUDEPATH += ../inc

DEFINES += _DEBUG

win32 {
    INCLUDEPATH += D:/boost/include
    LIBS += -LD:/boost/lib/x86/vc100
    DEF_FILE = ../vs/libstd.def
}

DEFINES += _QT

