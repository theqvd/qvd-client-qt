#include <QtTest>
#include <QCoreApplication>
#include "helpers/pathtools.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "usbip/usbdatabase.h"
#include "usbip/usbdevicelist.h"
#include "usbip/usbdevice.h"

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
    void listDevices();
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
#ifdef Q_OS_WIN
    QSKIP("Parsing database disabled on Windows");
#else
    UsbDatabase db;
    QBENCHMARK {
        QVERIFY( db.load() );
    }

    //QWARN( QString("Parsed with %1 vendors and %2 devices").arg(db.getVendorCount()).arg(db.getItemCount()).toUtf8() );

    // As of writing the DB contains 3441 vendors and 19950 devices.
    // This is just a sanity check to make sure things seem to be in the right ballpark.
    QVERIFY( db.getVendorCount() > 3000);
    QVERIFY( db.getItemCount() > 15000);
#endif
}

void tst_UsbIp::listDevices() {
    UsbDeviceList devList;

    QSignalSpy spy(&devList, &UsbDeviceList::updated);

    devList.refresh();
    QVERIFY(spy.wait());

    // QSignalSpy captures the signals emitted by the UsbDeviceList, as well as the arguments they're passed
    // First, get the arguments to the first event (only one in this case)
    QList<QVariant> arguments = spy.takeFirst();

    // Get the first argument ("bool success")
    auto first_argument = arguments.at(0);

    // Make sure it's indeed a bool, sanity check
    QVERIFY(first_argument.type() == QVariant::Bool);

    // Make sure it's true (actual test, make sure the update succeeded)
    QVERIFY(qvariant_cast<bool>(first_argument));


    auto devices = devList.getDevices();
    for(const auto& dev : qAsConst(devices)) {
        qDebug() << dev;
    }

    QVERIFY(devices.count() > 0);
}

QTEST_MAIN(tst_UsbIp)

#include "tst_usbip.moc"
