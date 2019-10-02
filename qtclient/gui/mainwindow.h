#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qvdclient.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QVDClient *m_client;

public:
    explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	void setUI(bool enabled);
    void closeEvent(QCloseEvent *event);

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
