#-------------------------------------------------
#
# Project created by QtCreator 2013-01-27T18:29:45
#
#-------------------------------------------------

QT       += core gui

TARGET = quick-logic-sniffer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lptrecorder.cpp \
    ftdirecorder.cpp \
    winioprovider.cpp \
    ftdiinterfaceprovider.cpp \
    oslproducer.cpp

HEADERS  += mainwindow.h \
    lptrecorder.h \
    ftdirecorder.h \
    winioprovider.h \
    ftdiinterfaceprovider.h \
    oslproducer.h

FORMS    += mainwindow.ui
