#include "commandlineparser.h"
#include <QMetaEnum>

#define _t(s) (QCoreApplication::translate("CommandLineParser", s))



CommandLineParser::CommandLineParser()
{

    m_qparser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);


}

QString CommandLineParser::getConnectionTypes(QVDConnectionParameters &params) const
{
    QString list;

    return list;
}

CommandLineParser::Result CommandLineParser::parse(QVDConnectionParameters &params) {

    const auto username       = addOption("user"           , _t("Login username."), "username");
    const auto password       = addOption("password"       , _t("Login password."), "password");
    const auto token          = addOption("token"          , _t("Login bearer token (used instead of password)."), "token");
    const auto host           = addOption("host"           , _t("Server to connect to."), "host");
    const auto port           = addOption("port"           , _t("Port QVD is running on."), "port");
    const auto file           = addOption("file"           , _t("Open file in VM."), "file");
    const auto ssl            = addOption("ssl"            , _t("Enable or disable the use of SSL. Default True."), "ssl");
    const auto sslerrors      = addOption("ssl-errors"     , _t("What to do in case of SSL errors. Valid values are: 'ask', 'continue' and 'abort'."), "ssl-errors");
    const auto listvms        = addOption("list-vms"       , _t("List VMs, and exit."), "list-vms");
    const auto listapps       = addOption("list-apps"      , _t("List applications, and exit."), "list-apps");
    const auto noheader       = addOption("no-header"      , _t("Don't show the header for --list-vms and --list-apps."), "no-header");
    const auto json           = addOption("json"           , _t("Use JSON to dump the --list-vms and --list-apps data."), "json");
    const auto connectionType = addOption("connection-type", _t("Type of connection to use"), "type");

    const auto helpOption = m_qparser.addHelpOption();
    const auto versionOption = m_qparser.addVersionOption();

    if (!m_qparser.parse(QCoreApplication::arguments())) {
        setError(m_qparser.errorText());
        return Result::CommandLineError;
    }

    if (m_qparser.isSet(versionOption))
        return Result::CommandLineVersionRequested;

    if (m_qparser.isSet(helpOption))
        return Result::CommandLineHelpRequested;

    if (m_qparser.isSet(username))
        params.setUsername(m_qparser.value(username));

    if (m_qparser.isSet(password))
        params.setPassword(m_qparser.value(password));

    if (m_qparser.isSet(token))
        params.setToken(m_qparser.value(token));

    if (m_qparser.isSet(host))
        params.setHost(m_qparser.value(host));

    if (m_qparser.isSet(port))
        params.setPort(m_qparser.value(port).toUInt());

    if (m_qparser.isSet(ssl))
        params.setPassword(m_qparser.value(ssl));

    if (m_qparser.isSet(sslerrors))
        params.setPassword(m_qparser.value(sslerrors));


    return Result::CommandLineOk;
}

QCommandLineOption CommandLineParser::addOption(const QString &name, const QString &description, QString valuename)
{
    QCommandLineOption ret(name, description, valuename);
    m_qparser.addOption(ret);
    return ret;
}

const QString CommandLineParser::getHelp() const
{
    return m_qparser.helpText();
}

