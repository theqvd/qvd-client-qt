#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T20:06:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QVD_Client
TEMPLATE = app
CONFIG += c++17


SOURCES += main.cpp \
    commandlineparser.cpp \
    configloader.cpp \
    mainwindow.cpp \
    sslerrordialog.cpp \
    usbdevicelistmodel.cpp

macx:SOURCES  += keyboard_detector/keyboarddetector_osx.cpp
linux:SOURCES += keyboard_detector/keyboarddetector_linux.cpp
win32:SOURCES += keyboard_detector/keyboarddetector_windows.cpp

HEADERS  += \
    commandlineparser.h \
    configloader.h \
    keyboard_detector/keyboarddetector.h \
    mainwindow.h \
    sslerrordialog.h \
    usbdevicelistmodel.h

FORMS    += \
    mainwindow.ui \
    sslerrordialog.ui

RESOURCES += \
    images.qrc

macx {
    ICON = pixmaps/qvd.icns
    QMAKE_INFO_PLIST = Info.plist
}

unix:LIBS += -lX11


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qvdclient/release/ -lqvdclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qvdclient/debug/ -lqvdclient
else:unix: LIBS += -L$$OUT_PWD/../qvdclient/ -lqvdclient

INCLUDEPATH += $$PWD/../qvdclient
DEPENDPATH += $$PWD/../qvdclient
