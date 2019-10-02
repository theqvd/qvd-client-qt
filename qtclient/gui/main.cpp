#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Defaults used by QSettings
    QCoreApplication::setOrganizationName("Qindel");
    QCoreApplication::setOrganizationDomain("qindel.com");
    QCoreApplication::setApplicationName("QVD Client");

    qSetMessagePattern("[%{type}] (%{file}:%{line}) %{message}");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
