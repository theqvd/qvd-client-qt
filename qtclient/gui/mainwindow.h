#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qvdclient.h>
#include "backends/qvdbackend.h"
#include "backends/qvdnxbackend.h"
#include "usbip/usbdevicelist.h"

#include "usbdevicelistmodel.h"

#include <qvdchecklistview.h>
#include "QSettings"
#include <QSsl>
#include <QList>
#include <QStringListModel>
#include <QTimer>

#include "movingaverage.h"
#include "connectionstatistics.h"
#include "commandlineparser.h"
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

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setUI(bool enabled);
    bool connectionActive();
    void closeEvent(QCloseEvent *event);
    void showEvent( QShowEvent *event);

    void setMiscParameters(CommandLineParser::MiscParameters misc_params);


public slots:
    void setConnectionParms(const QVDConnectionParameters &params);

    void connectToVM();
    void vmListReceived(const QList<QVDClient::VMInfo> &vmlist);
    void socketError(QAbstractSocket::SocketError  error);
    void connectionEstablished();
    void vmConnected(int id);
    void vmPoweredDown(int id);
    void connectionError(QVDClient::ConnectionError error, const QString &error_desc, const QMap<QString, QString>& headers);
    void connectionTerminated();
    void sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain, bool &continueConnection);
    void addSharedFolder();
    void removeSharedFolder();
    void enableSharedFoldersClicked();

    void twoFactorAuthenticationRequired(QVDClient::SecondFactorType type, int min_length, int max_length);
    void twoFactorEnrollment(const QVDClient::SecondFactorEnrollmentData &data);

private slots:
    void backendTrafficInc(int64_t in, int64_t out);
    void printTraffic();
    void updateTwoFactorField();

private:
    Ui::MainWindow *ui;
    void saveSettings();
    void loadSettings();
    void updateVersionInfo();
    QString getTimeZone();
    void setTabVisibility(QWidget *tab, bool visibility);


    QVDConnectionParameters m_params;
    QStringList m_shared_folders;
    QStringListModel m_shared_folders_model;
    QStringList m_environment_variables, m_system_evironments_variables;
    QVDListModel m_environment_variables_model;

    UsbDeviceList m_device_list;
    USBDeviceListModel m_usb_device_model;

    MovingAverage m_avg_in_15s{15000};
    MovingAverage m_avg_out_15s{15000};
    QTimer m_traffic_timer;
    ConnectionStatistics m_stats_window;
    CommandLineParser::MiscParameters m_misc_params;

    bool m_skip_auth_error_messagebox = false;

};

#endif // MAINWINDOW_H
