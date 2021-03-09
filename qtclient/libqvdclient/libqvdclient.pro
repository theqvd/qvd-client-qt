

TARGET=qvdclient
TEMPLATE=lib

# c++17 causes a problem on osx:
# /opt/X11/include/X11/XKBlib.h:399: error: ISO C++17 does not allow 'register' storage class specifier [-Wregister]
#    register KeySym *           /* sym_return */,
#    ^~~~~~~~~

CONFIG += c++17 create_prl shared
win32:CONFIG += staticlib

QT = core network gui

SOURCES += \
    backends/qvdbackend.cpp \
#    backends/qvdlibnxbackend.cpp \
    backends/qvdnxbackend.cpp \
    helpers/binaryfinder.cpp \
    helpers/linebuffer.cpp \
    helpers/portallocator.cpp \
    helpers/qvdfilehelpers.cpp \
    nxerrorcommanddata.cpp \
    qvdchecklistview.cpp \
    qvdconnectionparameters.cpp \
    qvdhttpserver.cpp \
    qvdhttp.cpp \
    qvdnetworkreply.cpp \
    slaveclient/commands/connectaudio.cpp \
    slaveclient/commands/qvdslavecommand.cpp \
    slaveclient/commands/shareusbdevice.cpp \
    slaveclient/qvdslaveclient.cpp \
    socketforwarder.cpp \
    qvdclient.cpp \
    usbip/qvdusbip.cpp \
    usbip/usbdevice.cpp

# Allow for building against nxlibs source that's
# checked out in the same directory as ours.
#INCLUDEPATH += "../../../nx-libs/nxcomp/include/"
#LIBS += "-L../../../nx-libs/nx-X11/exports/lib/"


unix:INCLUDEPATH += "/usr/include/nx"
macx:INCLUDEPATH += "/opt/X11/include"
macx:LIBS += "-L/opt/X11/lib"
macx:QMAKE_LFLAGS += "-L/opt/X11/lib"


macx:SOURCES += \
    backends/xserverlauncher_osx.cpp \
    slaveclient/commands/slavesharefolderwithvm.cpp \
    pulseaudio/pulseaudio.cpp

linux:SOURCES += \
    backends/xserverlauncher_linux.cpp \
    slaveclient/commands/slavesharefolderwithvm.cpp \
    pulseaudio/pulseaudio.cpp

win32:SOURCES += \
    backends/xserverlauncher_windows.cpp \
    slaveclient/commands/slavesharefolderwithvm_win.cpp \
    pulseaudio/pulseaudio_win.cpp

HEADERS  += \
    backends/qvdbackend.h \
#    backends/qvdlibnxbackend.h \
    backends/qvdnxbackend.h \
    backends/xserverlauncher.h \
    helpers/binaryfinder.h \
    helpers/linebuffer.h \
    helpers/portallocator.h \
    helpers/qvdfilehelpers.h \
    nxerrorcommanddata.h \
    pulseaudio/pulseaudio.h \
    qvdchecklistview.h \
    qvdconnectionparameters.h \
    qvdhttpserver.h \
    qvdhttp.h \
    qvdnetworkreply.h \
    slaveclient/commands/connectaudio.h \
    slaveclient/commands/qvdslavecommand.h \
    slaveclient/commands/shareusbdevice.h \
    slaveclient/commands/slavesharefolderwithvm.h \
    slaveclient/qvdslaveclient.h \
    socketforwarder.h \
    qvdclient.h \
    usbip/qvdusbip.h \
    usbip/usbdevice.h

win32:HEADERS +=
unix:HEADERS +=
macx:HEADERS +=

#unix:QMAKE_CXXFLAGS += -fsanitize=address -fsanitize=undefined
#unix:LIBS += -fsanitize=address -fsanitize=undefined

unix:QMAKE_CXXFLAGS += -ggdb

#LIBS += -lXcomp
win32:LIBS += -lkernel32 -lUser32

qvdclient.path = $$[QT_INSTALL_PREFIX]/lib64
INSTALLS += qvdclient
