

TARGET=qvdclient
TEMPLATE=lib
#CONFIG=dll

CONFIG += c++17

QT = core network

SOURCES += \
    backends/qvdbackend.cpp \
    backends/qvdnxbackend.cpp \
    qvdconnectionparameters.cpp \
    qvdhttpserver.cpp \
    qvdhttp.cpp \
    qvdnetworkreply.cpp \
    socketforwarder.cpp \
    util/qvdsysteminfo.cpp \
    qvdclient.cpp

unix:SOURCES += util/qvdsysteminfo_linux.cpp
win32:SOURCES += util/qvdsysteminfo_windows.cpp


HEADERS  += \
    backends/qvdbackend.h \
    backends/qvdnxbackend.h \
    qvdconnectionparameters.h \
    qvdhttpserver.h \
    qvdhttp.h \
    qvdnetworkreply.h \
    socketforwarder.h \
    util/qvdsysteminfo.h \
    qvdclient.h

win32:HEADERS += util/qvdsysteminfo_windows.h
unix:HEADERS += util/qvdsysteminfo_linux.h

win32:LIBS += -lkernel32 -lUser32
unix:LIBS += -lX11


