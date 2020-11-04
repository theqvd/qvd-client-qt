#QT += testlib network
#QT -= gui

#CONFIG += qt console warn_on depend_includepath testcase
#CONFIG -= app_bundle

#TEMPLATE = app

#SOURCES +=  tst_linebuffertest.cpp \
#    main.cpp \
#    tst_pulseaudiotest.cpp

QMAKE_RPATHDIR = ../qvdclient

#QMAKE_CXXFLAGS += -fsanitize=address -fsanitize=undefined
#unix:LIBS += -fsanitize=address -fsanitize=undefined

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qvdclient/release/ -lqvdclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qvdclient/debug/ -lqvdclient
else:unix: LIBS += -L$$OUT_PWD/../qvdclient/ -lqvdclient

INCLUDEPATH += $$PWD/../qvdclient
DEPENDPATH += $$PWD/../qvdclient

#HEADERS +=

TEMPLATE = subdirs

SUBDIRS += \
    linebuffer/linebuffer.pro \
    pulseaudio/pulseaudio.pro
