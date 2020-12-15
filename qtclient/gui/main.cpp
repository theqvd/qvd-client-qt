#include "mainwindow.h"
#include "configloader.h"
#include "commandlineparser.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>

// TODO: make this come from the build script
#define HAVE_SYSTEMD

#if defined(Q_OS_LINUX) && defined(HAVE_SYSTEMD)
#include <systemd/sd-journal.h>
#endif

void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray utf_msg = msg.toUtf8();
    QByteArray line_as_str = QString::number(context.line).toUtf8();

#if defined(Q_OS_LINUX) && defined(HAVE_SYSTEMD)
    switch(type) {
    case QtDebugMsg:
        sd_journal_print_with_location( LOG_DEBUG, context.file, line_as_str.constData(), context.function, "%s", utf_msg.constData());
        break;
    case QtInfoMsg:
        sd_journal_print_with_location( LOG_INFO, context.file, line_as_str.constData(), context.function, "%s", utf_msg.constData());
        break;
    case QtWarningMsg:
        sd_journal_print_with_location( LOG_WARNING, context.file, line_as_str.constData(), context.function, "%s", utf_msg.constData());
        break;
    case QtCriticalMsg:
        sd_journal_print_with_location( LOG_CRIT, context.file, line_as_str.constData(), context.function, "%s", utf_msg.constData());
        break;
    case QtFatalMsg:
        sd_journal_print_with_location( LOG_EMERG, context.file, line_as_str.constData(), context.function, "%s", utf_msg.constData());
        abort();
        break;
    }
//#else


#endif
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(LogHandler);

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


