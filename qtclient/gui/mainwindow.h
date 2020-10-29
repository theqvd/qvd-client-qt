#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qvdclient.h>
#include "backends/qvdbackend.h"
#include "backends/qvdnxbackend.h"
#include "usbdevicelistmodel.h"

#include <qvdchecklistview.h>
#include "QSettings"
#include <QSsl>
#include <QList>
#include <QStringListModel>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QVDClient *m_client;
    QVDBackend *m_backend = nullptr;
    QList<QString> items;

public:


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setUI(bool enabled);
    void closeEvent(QCloseEvent *event);

public slots:
    void setConnectionParms(const QVDConnectionParameters &params);

    void connectToVM();
    void vmListReceived(const QList<QVDClient::VMInfo> &vmlist);
    void socketError(QAbstractSocket::SocketError  error);
    void connectionEstablished();
    void connectionError(QVDClient::ConnectionError error, QString error_desc);
    void connectionTerminated();
    void sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain, bool &continueConnection);
    void addSharedFolder();
    void removeSharedFolder();
    void enableSharedFoldersClicked();
private:
    Ui::MainWindow *ui;
    void saveSettings();
    void loadSettings();

    QVDConnectionParameters m_params;
    QStringList m_shared_folders;
    QStringListModel m_shared_folders_model;
    QStringList m_environment_variables, m_system_evironments_variables;
    QVDListModel m_environment_variables_model;

    USBDeviceListModel m_usb_device_model;

};

#endif // MAINWINDOW_H
