#include <QtTest>
#include <QCoreApplication>
#include "helpers/binaryfinder.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "helpers/keyboarddetector.h"

class tst_KeyboardDetector : public QObject
{
    Q_OBJECT

public:
    tst_KeyboardDetector();
    virtual ~tst_KeyboardDetector();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void detectKeyboard();
};


tst_KeyboardDetector::tst_KeyboardDetector()
{

}

tst_KeyboardDetector::~tst_KeyboardDetector()
{
}

void tst_KeyboardDetector::initTestCase() {

}

void tst_KeyboardDetector::cleanupTestCase() {

}
void tst_KeyboardDetector::detectKeyboard()
{
    QString keyboard = KeyboardDetector::getKeyboardLayout();
    QVERIFY2( keyboard.contains("/"), "Ensure keyboard layout is syntactically correct" );
    QVERIFY2( !keyboard.isEmpty(), "Ensure the keyboard layout isn't an empty one");
    QVERIFY2( keyboard != "empty/empty", "Ensure it's not the hardcoded empty/empty default" );
    QWARN(QString("Detected layout: %1").arg(keyboard).toUtf8());
}

QTEST_MAIN(tst_KeyboardDetector)

#include "tst_keyboard_detector.moc"
