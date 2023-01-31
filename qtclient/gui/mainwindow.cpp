// Default dimensions to use. These ensure the size in the UI file doesn't matter.
const int MAIN_WINDOW_WIDTH = 438;
const int MAIN_WINDOW_HEIGHT = 544;



#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sslerrordialog.h"
#include "listdialog.h"
#include "ui_listdialog.h"
#include "ui_sslerrordialog.h"
#include "backends/qvdnxbackend.h"
#include "qvdconnectionparameters.h"


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
#include <QTimeZone>

#include "usbip/usbdevicelist.h"
#include "usbip/usbdevice.h"

#include "helpers/pathtools.h"
#include "helpers/keyboarddetector.h"

#include "versioninfo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_client = new QVDClient(this);

    QObject::connect(m_client, &QVDClient::socketError, this, &MainWindow::socketError);
    QObject::connect(m_client, &QVDClient::connectionEstablished, this, &MainWindow::connectionEstablished);
    QObject::connect(m_client, &QVDClient::vmListReceived, this, &MainWindow::vmListReceived);
    QObject::connect(m_client, &QVDClient::connectionError, this, &MainWindow::connectionError);
    QObject::connect(m_client, &QVDClient::connectionTerminated, this, &MainWindow::connectionTerminated);
    QObject::connect(m_client, &QVDClient::sslErrors, this, &MainWindow::sslErrors);
    QObject::connect(m_client, &QVDClient::vmConnected, this, &MainWindow::vmConnected);
    QObject::connect(m_client, &QVDClient::vmPoweredDown, this, &MainWindow::vmPoweredDown);


    ui->setupUi(this);

    setWindowIcon(QIcon(":/pixmaps/qvd.ico"));



    ui->connectionTypeComboBox->addItem( "Modem", QVDConnectionParameters::ConnectionSpeed::Modem );
    ui->connectionTypeComboBox->addItem( "ISDN", QVDConnectionParameters::ConnectionSpeed::ISDN );
    ui->connectionTypeComboBox->addItem( "ADSL", QVDConnectionParameters::ConnectionSpeed::ADSL );
    ui->connectionTypeComboBox->addItem( "WAN", QVDConnectionParameters::ConnectionSpeed::WAN );
    ui->connectionTypeComboBox->addItem( "LAN", QVDConnectionParameters::ConnectionSpeed::LAN );
    ui->connectionTypeComboBox->addItem( "Local", QVDConnectionParameters::ConnectionSpeed::Local );


    m_shared_folders_model.setStringList(m_shared_folders);
    ui->sharedFoldersList->setModel(&m_shared_folders_model);

    m_system_evironments_variables = QProcessEnvironment::systemEnvironment().keys();
    m_environment_variables_model.setStringList(m_system_evironments_variables);
    ui->EnvVarList->setModel(&m_environment_variables_model);
    ui->EnvVarList->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->sharedDevicesList->setModel(&m_usb_device_model);
    ui->sharedDevicesList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sharedDevicesList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_usb_device_model.setDeviceList(&m_device_list);
    m_device_list.refresh();

    // Ensure the first tab is selected
    ui->mainTabWidget->setCurrentWidget(ui->connectTab);
    ui->sharedResourcesTabWidget->setCurrentWidget(ui->SharedFolders);
#ifdef Q_OS_MACX
    // No USBIP on Mac currently
    ui->USBRedirection->setVisible(false);
    ui->enableUSBRedirectionCheck->setChecked(false);

    int usb_redir_index = ui->sharedResourcesTabWidget->indexOf(ui->USBRedirection);
    ui->sharedResourcesTabWidget->removeTab(usb_redir_index);
#endif

    connect(&m_traffic_timer, &QTimer::timeout, this, &MainWindow::printTraffic);




    // Ensure the window's size is the smallest possible
    resize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);

    updateVersionInfo();
    loadSettings();

    // Set default object focus
    ui->username->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * @brief Sets up the main UI
 *
 * When the main UI is enabled, we're not inside a connection. Login info can be edited.
 * When the main UI is disabled, we're in an active conection. The connection can be canceled.
 *
 * @param enabled Whether it's to be enabled
 */
void MainWindow::setUI(bool enabled) {

    // We want to enable/disable everything inside the connectionLayout
    for(int i=0;i<ui->connectionLayout->count(); ++i) {
        QWidget *w = ui->connectionLayout->itemAt(i)->widget();
        if (w) {
            w->setEnabled(enabled);
        }
    }

    ui->cancelButton->setEnabled(!enabled);

    if ( enabled ) {
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
        ui->connectButton->setEnabled(true);
        ui->cancelButton->setEnabled(false);
        m_traffic_timer.stop();
    }
    else
    {
        ui->progressBar->setMaximum(0);
        ui->progressBar->setValue(0);
        ui->connectButton->setEnabled(false);
        ui->cancelButton->setEnabled(true);
        m_traffic_timer.start(15000);
    }

}

bool MainWindow::connectionActive()
{
    return !ui->centralWidget->isEnabled();
}

void MainWindow::connectToVM() {
    qInfo() << "Connecting to " << ui->serverLineEdit->text();

    setUI(false);
    QSettings settings;

    saveSettings();

    if (ui->serverLineEdit->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, tr("QVD"), tr("Server is not set.\nPlease configure a server to connect to."));
        setUI(true);
        m_stats_window.disconnectBackend();
        ui->mainTabWidget->setCurrentWidget(ui->settingsTab);
        ui->serverLineEdit->setFocus(Qt::FocusReason::OtherFocusReason);
        return;
    }


    auto speed = static_cast<QVDConnectionParameters::ConnectionSpeed>( ui->connectionTypeComboBox->currentData().toInt() );

    settings.beginGroup("paths");

    QVDNXBackend *nx_backend = new QVDNXBackend(this);
    connect(nx_backend, &QVDBackend::totalTrafficIncrement, this, &MainWindow::backendTrafficInc);

    settings.endGroup();

    settings.beginGroup("Connection");
    QVDConnectionParameters params;

    if ( ui->enablePrintingCheck->isChecked() ) {
        params.setPrinting(true);
    }

    if ( ui->fullScreenCheck->isChecked() ) {
        params.setFullscreen(true);
    }

    if ( ui->enableSharedFoldersCheck->isChecked() ) {
        params.setSharedFolders(m_shared_folders);
    }

    params.setKeyboard( KeyboardDetector::getKeyboardLayout() );
    
    
    if (ui->enableTwoFactorAuthCheck->isChecked()) {
        params.setSecondFactor(ui->secondFactor->text());
    } else {
        params.setSecondFactor("");
    }

    QString host = ui->serverLineEdit->text();

    if ( host.contains(':') ) {
         params.setHost(host.split(':')[0]);
         params.setPort( quint16( settings.value("port", host.split(':')[1]).toInt() ));
    } else {
         params.setHost( ui->serverLineEdit->text() );
         params.setPort( quint16( settings.value("port", "8443").toInt() ));
   }
        
    params.setConnectionSpeed(speed);

    params.setUsb_forwarding( ui->enableUSBRedirectionCheck->isChecked() );
    params.setSharedUsbDevices( m_usb_device_model.getSelectedDevices() );
    params.setAudio( ui->enableAudioCheck->isChecked() );
    params.setMicrophone( ui->enableMicrophoneCheck->isChecked() );
    params.setTimezone(getTimeZone());

    params.setUsername(ui->username->text());
    params.setPassword(ui->password->text());
    
    if(settings.contains("nxproxy_extra_args")) {
       params.setNxproxy_extra_args(settings.value("nxproxy_extra_args").toString());
    }

    if(settings.contains("nxagent_extra_args")) {
       params.setNxagent_extra_args(settings.value("nxagent_extra_args").toString());
    }

    if(settings.contains("geometry")) {
       params.setGeometry(QSize(settings.value("geometry").toSize()));
    }

    settings.endGroup();

    qInfo() << "Connecting with parameters " << params;

    nx_backend->setParameters(params);
    m_client->setBackend(nx_backend);
    m_stats_window.connectToBackend(nx_backend);
    m_client->setParameters(params);
    m_client->connectToQVD();
    //setUI(false);

//    ui->progressBar->setMinimum(0);
//    ui->progressBar->setMaximum(0);
}

void MainWindow::vmListReceived(const QList<QVDClient::VMInfo> &vmlist)
{
     int selected = -1;

     if ( vmlist.length() == 1 ) {
         selected = vmlist[0].id;
         qDebug() << "Only one VM available, auto-connecting to VM id " << selected;
     } else {
         ListDialog *dlg = new ListDialog(this);
         dlg->resize(350, 150);
         dlg->DisplayVMs(vmlist);
         dlg->exec();
         selected = dlg->result();
    }

    if ( selected > 0 ) {
        if ( ui->restartSession->isChecked() ) {
            ui->restartSession->setChecked(false);
            m_client->stopVM( selected );
        } else {
            m_client->connectToVM( selected );
        }
    } else {
        m_client->disconnectFromQVD();
    }

}

void MainWindow::socketError(QAbstractSocket::SocketError error)
{

    qWarning() << "socketError " << error << " connecting with " << m_client->getParameters();
    qWarning() << "Socket error: " << m_client->getSocket()->errorString();
    qWarning() << "Socket error: " << m_client->getSocket()->error();

    if ( ! connectionActive() ) {
        // This ensures the error only happens once
        QString message = QString("Failed to connect to QVD at %1:%2\n%3")
                                .arg(m_client->getParameters().host())
                                .arg(m_client->getParameters().port())
                                .arg(m_client->getSocket()->errorString());

        this->show();
	QMessageBox::critical(this, tr("QVD"), message);

        setUI(true);
        m_stats_window.disconnectBackend();
    }
}

void MainWindow::connectionEstablished()
{
    m_client->requestVMList();
}

void MainWindow::vmConnected(int id)
{
    qInfo() << "Connection to VM " << id << "established";
    this->hide();
}

void MainWindow::vmPoweredDown(int id)
{
    qInfo() << "VM " << id << "was powered down";
    m_client->connectToVM(id);
}

void MainWindow::connectionError(QVDClient::ConnectionError error, QString error_desc)
{
    QString errstr;

    switch(error) {
    case QVDClient::ConnectionError::None: errstr = tr("Bug: no error"); break;
    case QVDClient::ConnectionError::AuthenticationError: errstr = tr("Authentication error"); break;
    case QVDClient::ConnectionError::ProtocolError: errstr = tr("Protocol error"); break;
    case QVDClient::ConnectionError::Unexpected: errstr = tr("Internal error"); break;
    case QVDClient::ConnectionError::Timeout: errstr = tr("Timeout"); break;
    case QVDClient::ConnectionError::VMStartError: errstr = tr("Error when starting the VM"); break;
    case QVDClient::ConnectionError::ServerBlocked: errstr = tr("Server blocked"); break;
    case QVDClient::ConnectionError::ServerError: errstr = tr("Server error"); break;
    case QVDClient::ConnectionError::XServerError: errstr = tr("X Server error"); break;
    case QVDClient::ConnectionError::BackendError: errstr = tr("Backend error"); break;

    }

    qCritical() << "Connection error " << error << ": " << error_desc;

    QMessageBox::critical(this, tr("QVD"), errstr + "\n" + error_desc);

    setUI(true);
    m_stats_window.disconnectBackend();

}

void MainWindow::connectionTerminated()
{
    qInfo() << "Connection terminated";
    setUI(true);
    m_stats_window.disconnectBackend();
    show();
}

void   MainWindow::sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain, bool &continueConnection)
{
    QList<QString> certList;
    QSet<QString> nonAcceptedHashes;

    qInfo() << "In sslErrors";

    for(auto& cert : cert_chain ) {
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

            for( auto& hash : certList ) {
                settings.setArrayIndex(i++);
                settings.setValue("hash", hash);
            }

            for( auto& hash : nonAcceptedHashes ) {
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
    QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder to share"), QDir::home().path());
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

void MainWindow::cancelButtonClicked()
{
    qWarning() << "Cancelling active connection";
    m_client->disconnectFromQVD();
}

void MainWindow::backendTrafficInc(int64_t in, int64_t out)
{
    m_avg_in_15s.add( in );
    m_avg_out_15s.add( out );
}

void MainWindow::printTraffic()
{
    qInfo() << "Traffic avg in =" << m_avg_in_15s.getAverage() << "bytes/s; out =" << m_avg_out_15s.getAverage() << "bytes/s";
}

void MainWindow::saveSettings() {
    QSettings settings;

    qDebug() << "Saving settings";

    settings.beginGroup("UI");

    writeDefaultSetting(settings, "settingsTabVisible", true);
    writeDefaultSetting(settings, "versionTabVisible", true);
    writeDefaultSetting(settings, "rememberPasswordVisible", true);
    writeDefaultSetting(settings, "restartSessionVisible", true);

    settings.endGroup();

    settings.beginGroup("Connection");

    QString host = ui->serverLineEdit->text();
    
    if ( ! host.isEmpty() ) {
        if ( host.contains(":") ) {
            settings.setValue("host", host.split(':')[0]);
            settings.setValue("port", host.split(':')[1]);
        } else {
            settings.setValue("host", ui->serverLineEdit->text());
            settings.setValue("port", "8443");
        }
    }

    settings.setValue("speed", ui->connectionTypeComboBox->currentData());
    settings.setValue("username", ui->username->text() );
    settings.setValue("remember_password", ui->rememberPasswordCheck->isChecked() );

    if ( ui->rememberPasswordCheck->isChecked() ) {
        settings.setValue("password", ui->password->text().toUtf8().toBase64() );
    } else {
        settings.setValue("password", "");
    }

    settings.setValue("enable_two_factor_auth", ui->enableTwoFactorAuthCheck->isChecked() );
    settings.setValue("enable_file_sharing", ui->enableSharedFoldersCheck->isChecked() );
    settings.setValue("enable_audio", ui->enableAudioCheck->isChecked() );
    settings.setValue("enable_microphone", ui->enableMicrophoneCheck->isChecked() );
    settings.setValue("enable_printing", ui->enablePrintingCheck->isChecked() );
    settings.setValue("fullscreen", ui->fullScreenCheck->isChecked() );

    settings.setValue("shared_folders", m_shared_folders);

    QStringList shared_devices;

    settings.setValue("enable_usb_redirection", ui->enableUSBRedirectionCheck->isChecked());

    settings.beginWriteArray("shared_usb_devices");
    int pos = 0;
    for( USBDevice &dev : this->m_usb_device_model.getSelectedDevices() ) {
        settings.setArrayIndex(pos);
        settings.setValue("vendor", dev.vendorId());
        settings.setValue("product", dev.productId());
        settings.setValue("serial", dev.serial());
        pos++;
    }

    settings.endArray();
    settings.endGroup();

}

void MainWindow::loadSettings() {
    QSettings settings;

    qDebug() << "Loading settings";

    settings.beginGroup("UI");
    bool settingsVisible = settings.value("settingsTabVisible", true).toBool();
    bool versionVisible = settings.value("versionTabVisible", true).toBool();
    bool rememberPasswordVisible = settings.value("rememberPasswordVisible", true).toBool();
    bool restartSessionVisible = settings.value("restartSessionVisible", true).toBool();

    setTabVisibility(ui->settingsTab, settingsVisible);
    setTabVisibility(ui->versionTab, versionVisible);

    ui->rememberPasswordCheck->setVisible(rememberPasswordVisible);
    ui->restartSession->setVisible(restartSessionVisible);

    settings.endGroup();

    settings.beginGroup("Connection");

    QString host = settings.value("host").toString();
    QString port = settings.value("port").toString(); 
    
    if ( ! host.isEmpty() ) {
         ui->serverLineEdit->setText(host + ":" + port);
    }

    for(int i=0;i<ui->connectionTypeComboBox->count(); i++) {
        ui->connectionTypeComboBox->setCurrentIndex(i);
        if ( ui->connectionTypeComboBox->currentData() == settings.value("speed", QVDConnectionParameters::ConnectionSpeed::ADSL ))
            break;
    }

    ui->enableTwoFactorAuthCheck->setChecked( settings.value("enable_two_factor_auth").toBool() );
    ui->enableSharedFoldersCheck->setChecked( settings.value("enable_file_sharing").toBool() );
    ui->enableAudioCheck->setChecked( settings.value("enable_audio").toBool() );
    ui->enableMicrophoneCheck->setChecked( settings.value("enable_microphone").toBool() );
    ui->enablePrintingCheck->setChecked( settings.value("enable_printing").toBool() );
    ui->fullScreenCheck->setChecked( settings.value("fullscreen").toBool() );

    m_shared_folders = settings.value("shared_folders").toStringList();
    m_shared_folders_model.setStringList(m_shared_folders);
    enableSharedFoldersClicked();

    ui->enableUSBRedirectionCheck->setCheckState( settings.value("enable_usb_redirection").toBool() ? Qt::Checked : Qt::Unchecked );

    int count = settings.beginReadArray("shared_usb_devices");
    for( int i = 0;i<count;i++) {
        settings.setArrayIndex(i);
        int vendor = settings.value("vendor").toInt();
        int product = settings.value("product").toInt();
        QString serial = settings.value("serial").toString();

        m_usb_device_model.selectDevice(vendor, product, serial);
    }
    settings.endArray();
    settings.endGroup();


    settings.endGroup();
    settings.beginGroup("Connection");

    ui->username->setText( settings.value("username").toString());
    ui->rememberPasswordCheck->setChecked( settings.value("remember_password").toBool());
    ui->password->setText(  QString::fromUtf8( QByteArray::fromBase64( settings.value("password").toByteArray() )));

    settings.endGroup();

    // Ensure the second factor auth field is appropriately visible or not
    on_enableTwoFactorAuthCheck_toggled(ui->enableTwoFactorAuthCheck->isChecked() );

    settings.endGroup();
}

void MainWindow::updateVersionInfo()
{
    QString verText = "";
    QTextStream verStr(&verText);

    if ( VersionInfo::isRunningFromSource() ) {
	verStr << tr("Running from source\n");
    } else {
        verStr << "QVD Client " << VersionInfo::getFullVersion() << "\n";
	verStr << QString::fromUtf8(u8"\u00a9 Qindel Group 2022\n\n");
        verStr << "Build " << VersionInfo::getBuild() << "\n";
    }

    if (!VersionInfo::getCommit().isEmpty() ) {
        verStr << "Git commit " << VersionInfo::getCommit() << "\n";
    }

    verStr.flush();

    ui->versionInfoEdit->document()->setPlainText(verText);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
//    if (settings_cert.value("Accept").toString() != "0")
//    {
        event->accept();
//    }
}

void MainWindow::setMiscParameters(CommandLineParser::MiscParameters misc_params)
{
    m_misc_params = misc_params;

    ui->showBandwidthGraphCheck->setVisible( m_misc_params.enableExperimentalFeatures );
}

void MainWindow::setConnectionParms(const QVDConnectionParameters &params)
{
    m_params = params;
    //ui->username->setText(m_params.username());
    //ui->password->setText(m_params.password());
    //ui->serverLineEdit->setText(m_params.host());
}

QString MainWindow::getTimeZone() {
    QString iana_id(QTimeZone::systemTimeZoneId());
    qInfo() << "Detected timezone: " << iana_id;
    return iana_id;
}

void MainWindow::setTabVisibility(QWidget *tab, bool visibility)
{
    for(int i=0;i<ui->mainTabWidget->count();i++) {
        if ( ui->mainTabWidget->widget(i) == tab) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            ui->mainTabWidget->setTabEnabled(i, visibility);
#else
            ui->mainTabWidget->setTabVisible(i, visibility);
#endif
        }
    }
}

void MainWindow::writeDefaultSetting(QSettings &settings, const QString &name, QVariant defaultValue)
{
    settings.setValue(name, settings.value(name, defaultValue));
}

void MainWindow::on_enableTwoFactorAuthCheck_toggled(bool checked)
{
    ui->secondFactorLabel->setVisible( checked );
    ui->secondFactor->setVisible( checked );
}
