#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sslerrordialog.h"
#include "ui_sslerrordialog.h"
#include "util/qvdsysteminfo.h"
#include "backends/qvdnxbackend.h"
#include "qvdconnectionparameters.h"

#include <QDebug>
#include <QMessageBox>
#include <QSslCertificate>
#include <QSettings>
#include <QIcon>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	m_client = new QVDClient(this);




	QObject::connect(m_client, SIGNAL(vmListReceived(QList<QVDClient::VMInfo>)), this, SLOT(vmListReceived(QList<QVDClient::VMInfo>)));
	QObject::connect(m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	QObject::connect(m_client, SIGNAL(connectionEstablished()), this, SLOT(connectionEstablished()));
	QObject::connect(m_client, SIGNAL(connectionError(QVDClient::ConnectionError,QString)), this, SLOT(connectionError(QVDClient::ConnectionError,QString)));
	QObject::connect(m_client, SIGNAL(sslErrors(QList<QSslError>, QList<QSslCertificate>)), this, SLOT(sslErrors(QList<QSslError>, QList<QSslCertificate>)));

    setWindowIcon(QIcon(":/pixmaps/qvd.ico"));

	ui->setupUi(this);


    ui->connectionTypeComboBox->addItem( "Local", QVDConnectionParameters::ConnectionSpeed::LAN );
    ui->connectionTypeComboBox->addItem( "ADSL", QVDConnectionParameters::ConnectionSpeed::ADSL );
    ui->connectionTypeComboBox->addItem( "Modem", QVDConnectionParameters::ConnectionSpeed::Modem );

    loadSettings();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setUI(bool enabled)
{
	ui->centralWidget->setEnabled(enabled);
	if ( enabled ) {
		ui->progressBar->setMaximum(100);
		ui->progressBar->setValue(0);
	}


}

void MainWindow::connect() {
    qInfo() << "Connecting to " << ui->serverLineEdit->text();

	setUI(false);
    QSettings settings;


    saveSettings();

    auto speed = static_cast<QVDConnectionParameters::ConnectionSpeed>( ui->connectionTypeComboBox->currentData().toInt() );

    settings.beginGroup("paths");

    QVDNXBackend *nx_backend = new QVDNXBackend(this);
    nx_backend->setNxproxyBinary(settings.value("nxproxy", "/usr/bin/nxproxy").toString());



    settings.endGroup();


    settings.beginGroup("Connection");
    QVDConnectionParameters params;
    params.setUsername(ui->username->text());
    params.setPassword(ui->password->text());
    params.setHost((ui->serverLineEdit->text()));
    params.setPort( quint16( settings.value("port", 8443).toInt() ));
    params.setConnectionSpeed(speed);

    m_client->setBackend(nx_backend);
    m_client->setParameters(params);
	m_client->connectToQVD();

	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(0);
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


    QString message = QString("Failed to connect to QVD at %1:%2\n%3")
                            .arg(m_client->getParameters().host())
                            .arg(m_client->getParameters().port())
                            .arg(m_client->getSocket()->errorString());

    QMessageBox::critical(this, "QVD", message);

	setUI(true);
}

void MainWindow::connectionEstablished()
{
	qInfo() << "Connection established";

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

void MainWindow::sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain)
{
	SSLErrorDialog *dlg = new SSLErrorDialog(this);
	dlg->displayErrors(errors, cert_chain);
	dlg->exec();
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
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
    event->accept();
}
