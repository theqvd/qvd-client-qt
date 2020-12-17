#include "mainwindow.h"
#include "configloader.h"
#include "commandlineparser.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QDir>
#include <QFile>

#include "helpers/binaryfinder.h"

// TODO: make this come from the build script
#define HAVE_SYSTEMD

#if defined(Q_OS_LINUX) && defined(HAVE_SYSTEMD)
#include <systemd/sd-journal.h>
#endif

QFile log_file;
QTextStream log_stream;
QTextStream out_stream(stdout, QIODevice::WriteOnly);
QTextStream err_stream(stderr, QIODevice::WriteOnly);



void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {


#if defined(Q_OS_LINUX) && defined(HAVE_SYSTEMD)
    QByteArray utf_msg = msg.toUtf8();

    QByteArray arg_file = QString("CODE_FILE=%1").arg(context.file).toUtf8();
    QByteArray arg_line = QString("CODE_LINE=%1").arg(context.line).toUtf8();
    QByteArray arg_func = QString("CODE_FUNC=%1").arg(context.function).toUtf8();

    //QString::number(context.line).toUtf8();
    int ret = 0;
    int journald_level = 0;


    switch(type) {
    case QtDebugMsg   : journald_level = LOG_DEBUG; break;
    case QtInfoMsg    : journald_level = LOG_INFO; break;
    case QtWarningMsg : journald_level = LOG_WARNING; break;
    case QtCriticalMsg: journald_level = LOG_CRIT; break;
    case QtFatalMsg:    journald_level = LOG_EMERG; break;
    }

    ret = sd_journal_print_with_location( journald_level, arg_file.constData(), arg_line.constData(), arg_func.constData(), "%s", utf_msg.constData());
    if ( ret != 0 ) {
        err_stream << "sd_journal_print_with_location failed with error " << ret << Qt::endl;
    }
#endif

    QString level;

    switch(type) {
    case QtDebugMsg   : level = " DEBUG"; break;
    case QtInfoMsg    : level = " INFO "; break;
    case QtWarningMsg : level = " WARN "; break;
    case QtCriticalMsg: level = " CRIT "; break;
    case QtFatalMsg:    level = " FATAL"; break;
    }

    QDateTime now = QDateTime::currentDateTime();

    log_stream << now.toString(Qt::ISODateWithMs) << level << " [" << context.file << ":" << context.line << "] " << msg << Qt::endl;
    err_stream << now.toString(Qt::ISODateWithMs) << level << " [" << context.file << ":" << context.line << "] " << msg << Qt::endl;
}

int main(int argc, char *argv[])
{
    QDir dir  = QDir::root();
    dir.mkpath(PathTools::getLogDir());

    QDir log_dir(PathTools::getLogDir());

    log_file.setFileName(log_dir.filePath("client.log"));
    if (!log_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        err_stream << "Failed to open log file " << log_file.fileName();
    }
    log_stream.setDevice(&log_file);

    qInstallMessageHandler(LogHandler);

    qInfo() << "Client started";
#ifdef QT_NO_DEBUG_OUTPUT
    qInfo() << "Debug output disabled at compile time";
#endif

    qRegisterMetaType<QVDConnectionParameters::ConnectionSpeed>();


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
        out_stream << parser.getError();
        out_stream << parser.getHelp();
        out_stream.flush();
        exit(1);
        break;
    case CommandLineParser::CommandLineHelpRequested:
        out_stream << parser.getHelp();
        out_stream.flush();
        exit(0);
        break;
    case CommandLineParser::CommandLineVersionRequested:
        out_stream << "0.1";
        exit(0);
    }


    MainWindow w;
    w.setConnectionParms(params);
    w.show();

    return a.exec();
}


