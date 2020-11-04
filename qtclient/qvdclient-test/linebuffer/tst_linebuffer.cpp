#include <QtTest>
#include <QCoreApplication>

#include "helpers/linebuffer.h"

// add necessary includes here

class tst_LineBuffer : public QObject
{
    Q_OBJECT

public:
    tst_LineBuffer();
    ~tst_LineBuffer();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void addEmpty();
    void addEmptyLine();
    void addPartialLine();
    void addFullLine();
    void addFullAndPartial();
    void concatenation();
    void multiline();
    void unixLineEnding();
    void windowsLineEnding();
    void addMultipleEmptyLines();
};

tst_LineBuffer::tst_LineBuffer()
{

}

tst_LineBuffer::~tst_LineBuffer()
{

}

void tst_LineBuffer::initTestCase()
{

}

void tst_LineBuffer::cleanupTestCase()
{

}

void tst_LineBuffer::addEmpty()
{
    LineBuffer lb;

    lb.add("");
    QVERIFY( !lb.hasLine() );
    QVERIFY( !lb.hasPartialLine() );
}

void tst_LineBuffer::addEmptyLine()
{
    LineBuffer lb;

    lb.add("\n");
    QVERIFY( lb.hasLine() );
    QVERIFY( !lb.hasPartialLine() );
    QCOMPARE( lb.getPartialLine(), "");
}


void tst_LineBuffer::addPartialLine()
{
    LineBuffer lb;

    lb.add("Hello");
    QVERIFY( !lb.hasLine() );
    QVERIFY( lb.hasPartialLine() );
    QCOMPARE( lb.getPartialLine(), "Hello");
}

void tst_LineBuffer::addFullLine()
{
    LineBuffer lb;

    lb.add("This is a line\n");
    QVERIFY( lb.hasLine() );
    QVERIFY( !lb.hasPartialLine() );
    QCOMPARE( lb.getLine(), "This is a line");
}

void tst_LineBuffer::addFullAndPartial()
{
    LineBuffer lb;

    lb.add("This is a line\nThis is a part");
    QVERIFY( lb.hasLine() );
    QVERIFY( lb.hasPartialLine() );
    QCOMPARE( lb.getLine(), "This is a line");
    QVERIFY( !lb.hasLine() );
    QVERIFY( lb.hasPartialLine() );
    QCOMPARE( lb.getPartialLine(), QString("This is a part"));
}

void tst_LineBuffer::concatenation()
{
    LineBuffer lb;

    lb.add("hello ");
    lb.add("world\n");
    QCOMPARE( lb.getLine(), QString("hello world"));
}


void tst_LineBuffer::multiline()
{
    LineBuffer lb;

    lb.add("one\ntwo\nthree\n");
    QCOMPARE( lb.getLine(), "one");
    QCOMPARE( lb.getLine(), "two");
    QCOMPARE( lb.getLine(), "three");
    QVERIFY( !lb.hasLine() );
    QVERIFY( !lb.hasPartialLine() );
}

void tst_LineBuffer::unixLineEnding()
{
    LineBuffer lb;

    lb.add("one\ntwo\n");
    QCOMPARE( lb.getLine(), "one");
    QCOMPARE( lb.getLine(), "two");
    QVERIFY( !lb.hasLine() );
    QVERIFY( !lb.hasPartialLine() );
}

void tst_LineBuffer::windowsLineEnding()
{
    LineBuffer lb;

    lb.add("one\r\ntwo\r\n");
    QCOMPARE( lb.getLine(), "one");
    QCOMPARE( lb.getLine(), "two");
    QVERIFY( !lb.hasLine() );
    QVERIFY( !lb.hasPartialLine() );
}

void tst_LineBuffer::addMultipleEmptyLines()
{
    LineBuffer lb;

    lb.add("one\n\ntwo\n");
    QCOMPARE( lb.getLine(), "one");
    QCOMPARE( lb.getLine(), "two");
    QVERIFY( !lb.hasLine() );
    QVERIFY( !lb.hasPartialLine() );
}

QTEST_MAIN(tst_LineBuffer)

#include "tst_linebuffer.moc"
