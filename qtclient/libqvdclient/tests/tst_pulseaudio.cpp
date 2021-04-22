#include <QtTest>
#include <QCoreApplication>

#include "pulseaudio/pulseaudio.h"


class tst_PulseAudio : public QObject
{
    Q_OBJECT

public:
    tst_PulseAudio();
    virtual ~tst_PulseAudio();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void getVersion();
};


tst_PulseAudio::tst_PulseAudio()
{

}

tst_PulseAudio::~tst_PulseAudio()
{
}

void tst_PulseAudio::initTestCase() {

}

void tst_PulseAudio::cleanupTestCase() {

}
void tst_PulseAudio::getVersion()
{
    PulseAudio pa;
    QSignalSpy spy(&pa, SIGNAL(started()));

    pa.connectExisting();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    QVERIFY( pa.getVersion() >= QVersionNumber(8,0,0));
    QVERIFY( pa.getVersion().segmentCount() >= 3);


    QString vermsg = QString("System PA version is %1").arg(pa.getVersion().toString());

    QWARN(vermsg.toUtf8());



}

QTEST_MAIN(tst_PulseAudio)

#include "tst_pulseaudio.moc"
