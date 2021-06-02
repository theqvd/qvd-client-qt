#include "mainwindow.h"
#include "configloader.h"
#include "commandlineparser.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QMap>
#include <QDesktopServices>

#include "helpers/pathtools.h"


#ifdef Q_OS_UNIX
#include <signal.h>
#endif



// TODO: make this come from the build script
//#define HAVE_SYSTEMD

#if defined(Q_OS_LINUX) && defined(HAVE_SYSTEMD)
#include <systemd/sd-journal.h>
#endif

QFile log_file;
QTextStream log_stream;
QTextStream out_stream(stdout, QIODevice::WriteOnly);
QTextStream err_stream(stderr, QIODevice::WriteOnly);


enum Action {
    None,
    Disconnect,
    Terminate
};

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
        err_stream << "sd_journal_print_with_location failed with error " << ret << "\n";
        err_stream.flush();
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

    log_stream << now.toString(Qt::ISODateWithMs) << level << " [" << context.file << ":" << context.line << "] " << msg << "\n";
    err_stream << now.toString(Qt::ISODateWithMs) << level << " [" << context.file << ":" << context.line << "] " << msg << "\n";

    // Avoid Qt::endl, which is missing in Qt 5.11 used on the Pi
    log_stream.flush();
    err_stream.flush();
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
    NXErrorCommandData nxerr;

    auto &parser = CommandLineParser::getInstance();
    auto retval = parser.parse(params, nxerr);

    QMap<QMessageBox::Button, Action> button_action_map;

    if ( nxerr.isComplete() ) {
        qInfo() << "Showing message of type " << nxerr.Type << " with title " << nxerr.Caption << ", content " << nxerr.Message;

        QMessageBox msgBox;
        msgBox.setText(nxerr.Message);

        // OSX ignores this, not a bug. Required by macOS guidelines.
        msgBox.setWindowTitle(nxerr.Caption);

        switch(nxerr.Type) {
            case NXErrorCommandData::DialogType::Unknown:
            case NXErrorCommandData::DialogType::OK:
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setIcon(QMessageBox::Information);

                button_action_map.insert(QMessageBox::Ok, Action::Terminate);
                break;
            case NXErrorCommandData::DialogType::Error:
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setIcon(QMessageBox::Warning);

                button_action_map.insert(QMessageBox::Ok, Action::Terminate);
                break;
            case NXErrorCommandData::DialogType::YesNo:
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setIcon(QMessageBox::Question);

                button_action_map.insert(QMessageBox::Yes, Action::Terminate);
                break;
            case NXErrorCommandData::DialogType::YesNoSuspend:
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.addButton("Suspend", QMessageBox::ApplyRole);
                msgBox.setIcon(QMessageBox::Question);

                button_action_map.insert(QMessageBox::Yes, Action::Terminate);
                button_action_map.insert(QMessageBox::NoButton, Action::Disconnect);
                break;
            case NXErrorCommandData::DialogType::Panic:
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setIcon(QMessageBox::Critical);

                button_action_map.insert(QMessageBox::Ok, Action::Terminate);
                break;
            case NXErrorCommandData::DialogType::Pulldown:
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setIcon(QMessageBox::Question);

                button_action_map.insert(QMessageBox::Ok, Action::Terminate);
                break;
            case NXErrorCommandData::DialogType::Quit:
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setIcon(QMessageBox::Question);

                button_action_map.insert(QMessageBox::Ok, Action::Terminate);
                break;

        }

        QMessageBox::StandardButton selection = static_cast<QMessageBox::StandardButton>(msgBox.exec());
        qDebug() << "Selection made: " << selection;

        if ( button_action_map.contains(selection)) {
            qDebug() << "Will execute action" << button_action_map[selection] << "on PID" << nxerr.ParentPID;

            if ( nxerr.ParentPID != 0 ) {
#ifdef Q_OS_UNIX
                switch(button_action_map[selection]) {
                case Action::Disconnect:
                    qInfo() << "Sending SIGHUP to " << nxerr.ParentPID;
                    kill(nxerr.ParentPID, SIGHUP);
                    break;
                case Action::Terminate:
                    qInfo() << "Sending SIGTERM to " << nxerr.ParentPID;
                    kill(nxerr.ParentPID, SIGTERM);
                    break;
                default:
                    qWarning() << "No action to perform";
                    /* nothing */
                    break;
                }
#else
                qWarning() << "Actions not implemented on this OS";
#endif
            } else {
                qWarning() << "Can't execute action without a PID";
            }
        }

        qDebug() << "Exit: NX message done";
        exit(0);
    }

    switch (retval) {
    case CommandLineParser::CommandLineOk:
        // nothing
        break;
    case CommandLineParser::CommandLineError:
        out_stream << parser.getError();
        out_stream << parser.getHelp();
        out_stream.flush();
        qDebug() << "Exit: command line parse error";
        exit(1);
        break;
    case CommandLineParser::CommandLineHelpRequested:
        out_stream << parser.getHelp();
        out_stream.flush();
        qDebug() << "Exit: help command";
        exit(0);
        break;
    case CommandLineParser::CommandLineVersionRequested:
        out_stream << "0.1";
        qDebug() << "Exit: version command";
        exit(0);
    }

#ifdef Q_OS_MACX
    if(qEnvironmentVariableIsEmpty("DISPLAY")) {
        qWarning() << "Running on MacOS with $DISPLAY set, XQuartz is not installed";
        QMessageBox msgbox;
        msgbox.setWindowTitle("QVD Client");
        msgbox.setText("QVD Client requires XQuartz for its functionality.\n"
                       "Click Ok to go to the XQuartz website to download it.yu");

        msgbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        QMessageBox::StandardButton selection = static_cast<QMessageBox::StandardButton>(msgbox.exec());
        qDebug() << "Selection made: " << selection;

        if ( selection == QMessageBox::StandardButton::Ok ) {
            QDesktopServices::openUrl(QString("https://www.xquartz.org"));
            exit(0);
        }

        exit(1);
    } else {
        qInfo() << "Running on MacOS, DISPLAY is set to " << qgetenv("DISPLAY") << ", all good.";
    }
#endif

    MainWindow w;
    w.setConnectionParms(params);
    w.show();

    return a.exec();
}


