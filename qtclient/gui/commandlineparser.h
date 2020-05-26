#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QCommandLineParser>
#include "qvdconnectionparameters.h"



class CommandLineParser
{
public:
    enum Result {
        CommandLineOk,
        CommandLineError,
        CommandLineVersionRequested,
        CommandLineHelpRequested
    };

    static CommandLineParser& getInstance() {
        static CommandLineParser parser;
        return parser;
    }

    QCommandLineOption addOption(const QString& name, const QString &description, QString valuename = QString());

    Result parse(QVDConnectionParameters &params);

    const QString& getError() const { return m_error_message; }
    const QString getHelp() const;

private:
    CommandLineParser();
    CommandLineParser(const CommandLineParser &) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;

    QString m_error_message;
    QCommandLineParser m_qparser;

    QString getConnectionTypes(QVDConnectionParameters &params) const;




    void setError(const QString &error) { m_error_message = error; }




};

#endif // COMMANDLINEPARSER_H
