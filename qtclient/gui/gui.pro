#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T20:06:36
#
#-------------------------------------------------

QT       += core gui network widgets
TARGET = QVD_Client
TEMPLATE = app
CONFIG += c++17 file_copies

linux:CONFIG += link_pkgconfig
linux:PKGCONFIG += libsystemd

SOURCES += main.cpp \
    commandlineparser.cpp \
    configloader.cpp \
    listdialog.cpp \
    mainwindow.cpp \
    sslerrordialog.cpp \
    usbdevicelistmodel.cpp \
    versioninfo.cpp

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
    usbdevicelistmodel.h \
    versioninfo.h

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

linux:LIBS += -lX11

# Link directly against libX11 on OSX, rather than specifying a library path.
# If we use -L/opt/X11/lib here, it may use /opt/X11/lib/libGL.dylib instead
# of /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
#
# This causes an error of 'dyld: Symbol not found: _gll_noop'.

macx:LIBS += /opt/X11/lib/libX11.dylib

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

    RC_ICONS = pixmaps/qvd.ico
}


###
### Version info
###
GIT_COMMIT = $$system(git rev-parse HEAD)
MAJOR = $$(QVD_VERSION_MAJOR)
MINOR = $$(QVD_VERSION_MINOR)
REVISION = $$(QVD_VERSION_REVISON)
BUILD = $$(QVD_BUILD)

!defined(MAJOR) {
    message("Version number not set")
    MAJOR = 4
    MINOR = 3
    REVISION = 0
}

!defined(BUILD ) {
    message("Build number not set")
    BUILD = 0
}




QMAKE_CXXFLAGS += -DQVD_COMMIT_HASH=\\\"$$GIT_COMMIT\\\" -DQVD_VERSION_MAJOR=$$MAJOR -DQVD_VERSION_MINOR=$$MINOR -DQVD_VERSION_REVISION=$$REVISION -DQVD_BUILD=$$BUILD


