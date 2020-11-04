
CONFIG += warn_on

QMAKE_RPATHDIR = ../../qvdclient

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../qvdclient/release/ -lqvdclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../qvdclient/debug/ -lqvdclient
else:unix: LIBS += -L$$OUT_PWD/../../qvdclient/ -lqvdclient

INCLUDEPATH += $$PWD/../qvdclient
DEPENDPATH += $$PWD/../qvdclient

