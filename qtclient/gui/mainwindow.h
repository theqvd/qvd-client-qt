#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qvdclient.h>
#include "backends/qvdbackend.h"
#include "backends/qvdnxbackend.h"
#include "QSettings"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QVDClient *m_client;
    QVDBackend *m_backend = nullptr;

public:
    explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	void setUI(bool enabled);
    void closeEvent(QCloseEvent *event);

    QSettings settings_cert;

public slots:
	void connect();
	void vmListReceived(const QList<QVDClient::VMInfo> &vmlist);
	void socketError(QAbstractSocket::SocketError  error);
	void connectionEstablished();
	void connectionError(QVDClient::ConnectionError error, QString error_desc);
	void sslErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain); 
private:
	Ui::MainWindow *ui;
    void saveSettings();
    void loadSettings();

};

#endif // MAINWINDOW_H
