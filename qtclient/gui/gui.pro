#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T20:06:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QVD_Client
TEMPLATE = app

INCLUDEPATH += ../qvdclient
LIBS += -L../qvdclient -lqvdclient
CONFIG += c++17


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    sslerrordialog.cpp

HEADERS  += \
    mainwindow.h \
    sslerrordialog.h

FORMS    += mainwindow.ui \
    sslerrordialog.ui

RESOURCES += \
    images.qrc

