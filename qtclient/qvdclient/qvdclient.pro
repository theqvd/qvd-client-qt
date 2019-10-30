

TARGET=qvdclient
TEMPLATE=lib
#CONFIG=dll

# c++17 causes a problem on osx:
# /opt/X11/include/X11/XKBlib.h:399: error: ISO C++17 does not allow 'register' storage class specifier [-Wregister]
#    register KeySym *           /* sym_return */,
#    ^~~~~~~~~

CONFIG += c++17



QT = core network gui

SOURCES += \
    backends/qvdbackend.cpp \
    backends/qvdnxbackend.cpp \
    qvdconnectionparameters.cpp \
    qvdhttpserver.cpp \
    qvdhttp.cpp \
    qvdnetworkreply.cpp \
    socketforwarder.cpp \
    qvdclient.cpp

macx:INCLUDEPATH += "/opt/X11/include"
macx:LIBS += "-L/opt/X11/lib"


macx:SOURCES += \
    backends/xserverlauncher_osx.cpp

linux:SOURCES += \
    backends/xserverlauncher_linux.cpp

win32:SOURCES += \
    backends/xserverlauncher_windows.cpp

HEADERS  += \
    backends/qvdbackend.h \
    backends/qvdnxbackend.h \
    backends/xserverlauncher.h \
    qvdconnectionparameters.h \
    qvdhttpserver.h \
    qvdhttp.h \
    qvdnetworkreply.h \
    socketforwarder.h \
    qvdclient.h

win32:HEADERS +=
linux:HEADERS +=
macx:HEADERS +=

win32:LIBS += -lkernel32 -lUser32
linux:LIBS += -lX11
