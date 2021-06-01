#include <QtTest>
#include <QCoreApplication>
#include "helpers/binaryfinder.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "usbip/usbdatabase.h"

class tst_UsbIp : public QObject
{
    Q_OBJECT

public:
    tst_UsbIp();
    virtual ~tst_UsbIp();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void parseDatabase();
};


tst_UsbIp::tst_UsbIp()
{

}

tst_UsbIp::~tst_UsbIp()
{
}

void tst_UsbIp::initTestCase() {

}

void tst_UsbIp::cleanupTestCase() {

}
void tst_UsbIp::parseDatabase()
{
    UsbDatabase db;
    QBENCHMARK {
        QVERIFY( db.load() );
    }

    //QWARN( QString("Parsed with %1 vendors and %2 devices").arg(db.getVendorCount()).arg(db.getItemCount()).toUtf8() );

    // As of writing the DB contains 3441 vendors and 19950 devices.
    // This is just a sanity check to make sure things seem to be in the right ballpark.
    QVERIFY( db.getVendorCount() > 3000);
    QVERIFY( db.getItemCount() > 15000);

}

QTEST_MAIN(tst_UsbIp)

#include "tst_usbip.moc"
