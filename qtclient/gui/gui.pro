#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T20:06:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QVD_Client
TEMPLATE = app
CONFIG += c++17 file_copies


SOURCES += main.cpp \
    commandlineparser.cpp \
    configloader.cpp \
    listdialog.cpp \
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
    listdialog.h \
    mainwindow.h \
    sslerrordialog.h \
    usbdevicelistmodel.h

FORMS    += \
    listdialog.ui \
    mainwindow.ui \
    sslerrordialog.ui

RESOURCES += \
    images.qrc

macx {
    ICON = pixmaps/qvd.icns
    QMAKE_INFO_PLIST = Info.plist
}

unix:LIBS += -lX11
macx:LIBS += -L/opt/X11/lib


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libqvdclient/release/ -lqvdclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libqvdclient/debug/ -lqvdclient
else:unix: LIBS += -L$$OUT_PWD/../libqvdclient/ -lqvdclient

INCLUDEPATH += $$PWD/../libqvdclient
DEPENDPATH += $$PWD/../libqvdclient

#unix:QMAKE_CXXFLAGS += -fsanitize=address -fsanitize=undefined
#unix:LIBS += -fsanitize=address -fsanitize=undefined

unix:QMAKE_CXXFLAGS += -ggdb -fPIE
unix:QMAKE_LFLAGS += -pie

DISTFILES += \
    config/config.xml \
    packages/com.qindel.qvd/meta/package.xml

program.path = $$[QT_INSTALL_PREFIX]/bin
program.files = QVD_Client
program.config = executable

unix:INSTALLS += program

win32 {
    ssl_libs.files = $$files(C:/Qt/Tools/OpenSSL/Win_x64/bin/*.dll)
    ssl_libs.path = $$OUT_PWD/debug
    COPIES += ssl_libs
}

