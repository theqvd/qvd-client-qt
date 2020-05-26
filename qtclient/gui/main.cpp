#include "mainwindow.h"
#include "configloader.h"
#include "commandlineparser.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>




int main(int argc, char *argv[])
{
    QTextStream out(stdout, QIODevice::WriteOnly);

    // Defaults used by QSettings
    QCoreApplication::setOrganizationName("Qindel");
    QCoreApplication::setOrganizationDomain("qindel.com");
    QCoreApplication::setApplicationName("QVD Client");

    qSetMessagePattern("[%{type}] (%{file}:%{line}) %{message}");

    QApplication a(argc, argv);

    QVDConnectionParameters params = ConfigLoader::loadConnectionParameters();
    auto &parser = CommandLineParser::getInstance();
    auto retval = parser.parse(params);


    switch (retval) {
    case CommandLineParser::CommandLineOk:
        // nothing
        break;
    case CommandLineParser::CommandLineError:
        out << parser.getError();
        out << parser.getHelp();
        out.flush();
        exit(1);
        break;
    case CommandLineParser::CommandLineHelpRequested:
        out << parser.getHelp();
        out.flush();
        exit(0);
        break;
    case CommandLineParser::CommandLineVersionRequested:
        out << "0.1";
        exit(0);
    }


    MainWindow w;
    w.setConnectionParms(params);
    w.show();

    return a.exec();
}


