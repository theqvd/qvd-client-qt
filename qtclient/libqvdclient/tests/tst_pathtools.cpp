#include <QtTest>
#include <QCoreApplication>
#include "helpers/pathtools.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

class tst_PathTools : public QObject
{
    Q_OBJECT

public:
    tst_PathTools();
    virtual ~tst_PathTools();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void logDir();
    void getPulseaudioHome();
    void getPulseaudioStateDir();
    void getPulseaudioBaseConfig();
    void getPulseaudioModulesDir();
    void findBin();

private:
    void verifyBin(const QString &bin);
};


tst_PathTools::tst_PathTools()
{

}

tst_PathTools::~tst_PathTools()
{
}

void tst_PathTools::initTestCase() {

}

void tst_PathTools::cleanupTestCase() {

}
void tst_PathTools::logDir()
{
    QFileInfo logdir(PathTools::getLogDir());
    QVERIFY( logdir.exists() );
    QVERIFY( logdir.isDir() );
    QVERIFY( logdir.isWritable() );
    QWARN( logdir.absolutePath().toUtf8() );
}

void tst_PathTools::getPulseaudioHome()
{
    QFileInfo pahome(PathTools::getPulseaudioHome());
    QVERIFY( pahome.exists() );
    QVERIFY( pahome.isDir() );
    QVERIFY( pahome.isWritable() );
    QWARN( pahome.absolutePath().toUtf8() );

}

void tst_PathTools::getPulseaudioStateDir()
{
    QFileInfo pastate(PathTools::getPulseaudioStateDir());
    QVERIFY( pastate.exists() );
    QVERIFY( pastate.isDir() );
    QVERIFY( pastate.isWritable() );
    QWARN( pastate.absolutePath().toUtf8() );
}

void tst_PathTools::getPulseaudioBaseConfig()
{
    QFileInfo pastate(PathTools::getPulseaudioBaseConfig());
    QVERIFY( pastate.exists() );
    QVERIFY( !pastate.isDir() );
    QVERIFY( pastate.isReadable() );
    QWARN( pastate.absolutePath().toUtf8() );
}

void tst_PathTools::getPulseaudioModulesDir()
{
    QFileInfo moddir(PathTools::getPulseaudioModuleDir());
    QVERIFY( moddir.exists());
    QVERIFY( moddir.isDir());
    QWARN( moddir.absolutePath().toUtf8());
}



void tst_PathTools::findBin()
{
    verifyBin("pulseaudio");
    verifyBin("nxproxy");
    verifyBin("sftp-server");

}

void tst_PathTools::verifyBin(const QString &bin)
{
    QString path = PathTools::findBin(bin);
    QFileInfo info(path);

    QVERIFY( !path.isEmpty() );
    QVERIFY( info.exists() );
    QVERIFY( info.isReadable() );
    QVERIFY( info.isExecutable() );
}

QTEST_MAIN(tst_PathTools)

#include "tst_pathtools.moc"
