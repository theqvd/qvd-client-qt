#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sslerrordialog.h"
#include "ui_sslerrordialog.h"
#include "backends/qvdnxbackend.h"
#include "qvdconnectionparameters.h"
#include "keyboard_detector/keyboarddetector.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QMessageBox>
#include <QSslCertificate>
#include <QSettings>
#include <QIcon>
#include <QFileDialog>
#include <QStringListModel>
#include <QStandardItemModel>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
    m_client = new QVDClient(this);
    QObject::connect(m_client, SIGNAL(vmListReceived(QList<QVDClient::VMInfo>)), this, SLOT(vmListReceived(QList<QVDClient::VMInfo>)));
    QObject::connect(m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    QObject::connect(m_client, SIGNAL(connectionEstablished()), this, SLOT(connectionEstablished()));
    QObject::connect(m_client, SIGNAL(connectionError(QVDClient::ConnectionError,QString)), this, SLOT(connectionError(QVDClient::ConnectionError,QString)));

    QObject::connect(m_client, SIGNAL(sslErrors(const QList<QSslError> &, const QList<QSslCertificate> &, bool &)), this, SLOT(sslErrors(const QList<QSslError> &, const QList<QSslCertificate> &, bool &)));
    QObject::connect(m_client, SIGNAL(connectionTerminated()), this, SLOT(connectionTerminated()));

    ui->setupUi(this);

    setWindowIcon(QIcon(":/pixmaps/qvd.ico"));

    ui->connectionTypeComboBox->addItem( "Local", QVDConnectionParameters::ConnectionSpeed::LAN );
    ui->connectionTypeComboBox->addItem( "ADSL", QVDConnectionParameters::ConnectionSpeed::ADSL );
    ui->connectionTypeComboBox->addItem( "Modem", QVDConnectionParameters::ConnectionSpeed::Modem );

    m_shared_folders_model.setStringList(m_shared_folders);
    ui->sharedFoldersList->setModel(&m_shared_folders_model);

    m_system_evironments_variables = QProcessEnvironment::systemEnvironment().keys();
    m_environment_variables_model.setStringList(m_system_evironments_variables);
    ui->EnvVarList->setModel(&m_environment_variables_model);
    ui->EnvVarList->setSelectionMode(QAbstractItemView::SingleSelection);

    loadSettings();
}

MainWindow::~MainWindow()
{
	delete ui;
}

MainWindow::CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString *errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption username("user", "Login username.", "username");
    parser.addOption(username);
    const QCommandLineOption password("password", "Login password.", "password");
    parser.addOption(password);
    const QCommandLineOption token("token", "Login bearer token (used instead of password).", "token");
    parser.addOption(token);
    const QCommandLineOption host("host", "Server to connect to.", "host");
    parser.addOption(host);
    const QCommandLineOption port("port", "Port QVD is running on.", "port");
    parser.addOption(port);
    const QCommandLineOption file("file", "Open file in VM.", "file");
    parser.addOption(file);
    const QCommandLineOption ssl("ssl", "Enable or disable the use of SSL. Default True.", "ssl");
    parser.addOption(ssl);
    const QCommandLineOption sslerrors("ssl-errors", "What to do in case of SSL errors. Valid values are: 'ask', 'continue' and 'abort'.", "ssl-errors");
    parser.addOption(sslerrors);
    const QCommandLineOption listvms("list-vms", "List VMs, and exit.", "list-vms");
    parser.addOption(listvms);
    const QCommandLineOption listapps("list-apps", "List applications, and exit.", "list-apps");
    parser.addOption(listapps);
    const QCommandLineOption noheader("no-header", "Don't show the header for --list-vms and --list-apps.", "no-header");
    parser.addOption(noheader);
    const QCommandLineOption json("json", "Use JSON to dump the --list-vms and --list-apps data.", "json");
    parser.addOption(json);

    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments())) {
        *errorMessage = parser.errorText();
        return MainWindow::CommandLineParseResult::CommandLineError;
    }

    if (parser.isSet(versionOption))
        return MainWindow::CommandLineParseResult::CommandLineVersionRequested;

    if (parser.isSet(helpOption))
        return MainWindow::CommandLineParseResult::CommandLineHelpRequested;

    if (!(parser.isSet(username) && parser.isSet(host))) {
        *errorMessage = parser.errorText();
        return MainWindow::CommandLineParseResult::CommandLineError;
    }

    if (!(parser.isSet(password) or (parser.isSet(token)))) {
        *errorMessage = parser.errorText();
        return MainWindow::CommandLineParseResult::CommandLineError;
    }

    if (parser.isSet(password) && parser.isSet(token)) {
        *errorMessage = parser.errorText();
        return MainWindow::CommandLineParseResult::CommandLineError;
    }

    return MainWindow::CommandLineParseResult::CommandLineOk;
}

void MainWindow::setUI(bool enabled)
{
	ui->centralWidget->setEnabled(enabled);
	if ( enabled ) {
		ui->progressBar->setMaximum(100);
		ui->progressBar->setValue(0);
	}
    else
    {
        ui->progressBar->setMaximum(0);
        ui->progressBar->setValue(0);
    }

}

void MainWindow::connectToVM() {
    qInfo() << "Connecting to " << ui->serverLineEdit->text();

	setUI(false);
    QSettings settings;

    saveSettings();

    auto speed = static_cast<QVDConnectionParameters::ConnectionSpeed>( ui->connectionTypeComboBox->currentData().toInt() );

    settings.beginGroup("paths");

    QVDNXBackend *nx_backend = new QVDNXBackend(this);


#if defined(Q_OS_WIN)
    // TODO: Detect location of nxproxy
    nx_backend->setNxproxyBinary("%TMP%\\nxproxy");
#elif defined(Q_OS_MACOS)
    nx_backend->setNxproxyBinary("/Applications/Qvd.app/Contents/Resources/usr/lib/qvd/bin/nxproxy");
#else
    nx_backend->setNxproxyBinary(settings.value("nxproxy", "/usr/bin/nxproxy").toString());
#endif

    settings.endGroup();

    settings.beginGroup("Connection");
    QVDConnectionParameters params;

    if ( ui->enableSharedFoldersCheck->isChecked() ) {
        params.setSharedFolders(m_shared_folders);
    }

    params.setKeyboard( KeyboardDetector::getKeyboardLayout() );
    params.setUsername(ui->username->text());
    params.setPassword(ui->password->text());
    params.setHost((ui->serverLineEdit->text()));
    params.setPort( quint16( settings.value("port", 8443).toInt() ));
    params.setConnectionSpeed(speed);

    qInfo() << "Connecting with parameters " << params;

    m_client->setBackend(nx_backend);
    m_client->setParameters(params);
    m_client->connectToQVD();
    //setUI(false);

//    ui->progressBar->setMinimum(0);
//    ui->progressBar->setMaximum(0);
}

void MainWindow::vmListReceived(const QList<QVDClient::VMInfo> &vmlist)
{

    int selected = -1;

	for(auto vm : vmlist ) {
		qInfo() << "VM id = " << vm.id << "; name = " << vm.name << "; blocked = " << vm.blocked << "; state = " << vm.state;

        if ( vm.state == QVDClient::VMState::Running ) {
            selected = vm.id;
        }
	}

    if ( vmlist.length() >= 1 ) {
        qInfo() << "Connecting to first VM, id " << vmlist.first().id;
        selected = vmlist.first().id;
	}

    if ( selected >= 0 ) {
        qInfo() << "Connecting to id " << selected;
        m_client->connectToVM( selected );
    } else {
        m_client->disconnect();
        qCritical() << "No VM to connect to";
        QMessageBox::critical(this, "QVD", "Couldn't find a VM to connect to");
    }
}

void MainWindow::socketError(QAbstractSocket::SocketError error)
{
    qWarning() << "socketError " << error << " connecting with " << m_client->getParameters();
    qWarning() << "Socket error: " << m_client->getSocket()->errorString();
    qWarning() << "Socket error: " << m_client->getSocket()->error();



    QString message = QString("Failed to connect to QVD at %1:%2\n%3")
                            .arg(m_client->getParameters().host())
                            .arg(m_client->getParameters().port())
                            .arg(m_client->getSocket()->errorString());

    QMessageBox::critical(this, "QVD", message);

	setUI(true);
}

void MainWindow::connectionEstablished()
{
	m_client->requestVMList();
}

void MainWindow::connectionError(QVDClient::ConnectionError error, QString error_desc)
{
	QString errstr;

	switch(error) {
	case QVDClient::ConnectionError::None: errstr = "Bug: no error"; break;
	case QVDClient::ConnectionError::AuthenticationError: errstr = "Authentication error"; break;
	case QVDClient::ProtocolError: errstr = "Protocol error"; break;
	case QVDClient::Unexpected: errstr = "Internal error"; break;
	case QVDClient::Timeout: errstr = "Timeout"; break;
	case QVDClient::VMStartError: errstr = "Error when starting the VM"; break;
	case QVDClient::ServerBlocked: errstr = "Server blocked"; break;
    case QVDClient::ServerError: errstr = "Server error"; break;
	}

    qCritical() << "Connection error " << error << ": " << error_desc;

	QMessageBox::critical(this, "QVD", errstr + "\n" + error_desc);

	setUI(true);

}

void MainWindow::connectionTerminated()
{
    qInfo() << "Connection terminated";
    setUI(true);
}

void   MainWindow::sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain, bool &continueConnection)
//void MainWindow::sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain)
{
    QList<QString> certList;
    QSet<QString> nonAcceptedHashes;

    qInfo() << "In sslErrors";

    for(auto cert : cert_chain ) {
        QString hash = cert.digest(QCryptographicHash::Sha256 ).toHex();

        qInfo() << "Hash: " << hash;
        nonAcceptedHashes.insert(hash);
    }

    QSettings settings;

    settings.beginGroup("SSL");
    int size = settings.beginReadArray("AcceptedCertificates");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString stored_hash = settings.value("hash").value<QString>();
        certList.append(stored_hash);

        qInfo() << "STORED: " << stored_hash;

        if ( nonAcceptedHashes.contains( stored_hash ) ) {
            nonAcceptedHashes.remove( stored_hash );
        }
    }
    settings.endArray();
    settings.endGroup();


    if ( nonAcceptedHashes.empty() ) {
        qInfo() << "All certs in the chain have been previously accepted";
        continueConnection = true;
    } else {
        SSLErrorDialog *dlg = new SSLErrorDialog(this);
        dlg->resize(700, 380);

        dlg->displayErrors(errors, cert_chain);
        dlg->exec();

        qInfo() << "Dialog result: " << dlg->result();

        continueConnection = (dlg->result() > 0);

        if ( dlg->result() == 2 ) {
            // Save to config
            settings.beginGroup("SSL");
            settings.beginWriteArray("AcceptedCertificates");
            int i=0;

            for( auto hash : certList ) {
                settings.setArrayIndex(i++);
                settings.setValue("hash", hash);
            }

            for( auto hash : nonAcceptedHashes ) {
                settings.setArrayIndex(i++);
                settings.setValue("hash", hash);
            }

            settings.endArray();
            settings.endGroup();
        }
    }

}

void MainWindow::addSharedFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select a folder to share", QDir::home().path());
    m_shared_folders.append(folder);
    m_shared_folders.removeDuplicates();
    m_shared_folders_model.setStringList(m_shared_folders);
}

void MainWindow::removeSharedFolder()
{
    auto selmodel = ui->sharedFoldersList->selectionModel();
    auto current = selmodel->currentIndex();

    if ( current.isValid() ) {
        m_shared_folders.removeAt(current.row());
        m_shared_folders_model.setStringList(m_shared_folders);
    }
}


void MainWindow::enableSharedFoldersClicked()
{
    bool enabled = ui->enableSharedFoldersCheck->checkState() == Qt::CheckState::Checked;
    ui->sharedFoldersList->setEnabled( enabled );
    ui->addSharedFolderButton->setEnabled( enabled );
    ui->removeSharedFolderButton->setEnabled( enabled );
}

void MainWindow::saveSettings() {
    QSettings settings;

    qDebug() << "Saving settings";

    settings.beginGroup("Connection");
    settings.setValue("host", ui->serverLineEdit->text() );
    settings.setValue("speed", ui->connectionTypeComboBox->currentData());
    settings.setValue("username", ui->username->text() );
    settings.setValue("remember_password", ui->rememberPasswordCheck->isChecked() );

    if ( ui->rememberPasswordCheck->isChecked() ) {
        settings.setValue("password", ui->password->text().toUtf8().toBase64() );
    } else {
        settings.setValue("password", "");
    }

    settings.setValue("enable_file_sharing", ui->enableSharedFoldersCheck->isChecked() );
    settings.setValue("shared_folders", m_shared_folders);
}

void MainWindow::loadSettings() {
    QSettings settings;

    qDebug() << "Loading settings";

    settings.beginGroup("Connection");

    ui->serverLineEdit->setText( settings.value("host", "").toString());

    for(int i=0;i<ui->connectionTypeComboBox->count(); i++) {
        ui->connectionTypeComboBox->setCurrentIndex(i);
        if ( ui->connectionTypeComboBox->currentData() == settings.value("speed", QVDConnectionParameters::ConnectionSpeed::LAN ))
            break;
    }

    ui->username->setText( settings.value("username").toString());
    ui->rememberPasswordCheck->setChecked( settings.value("remember_password").toBool());
    ui->password->setText(  QString::fromUtf8( QByteArray::fromBase64( settings.value("password").toByteArray() )));

    ui->enableSharedFoldersCheck->setChecked(  settings.value("enable_file_sharing").toBool() );
    m_shared_folders = settings.value("shared_folders").toStringList();
    m_shared_folders_model.setStringList(m_shared_folders);
    enableSharedFoldersClicked();



}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
//    if (settings_cert.value("Accept").toString() != "0")
//    {
        event->accept();
//    }
}


