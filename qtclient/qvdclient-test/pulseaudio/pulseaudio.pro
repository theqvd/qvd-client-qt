include($$PWD/../link_qvdclient.pri)



CONFIG += testcase
TARGET = tst_pulseaudio
QT = core testlib network
SOURCES += tst_pulseaudio.cpp


#QMAKE_RPATHDIR = ../../qvdclient

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../qvdclient/release/ -lqvdclient
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../qvdclient/debug/ -lqvdclient
#else:unix: LIBS += -L$$OUT_PWD/../../qvdclient/ -lqvdclient

#INCLUDEPATH += $$PWD/../../qvdclient
#DEPENDPATH += $$PWD/../../qvdclient
