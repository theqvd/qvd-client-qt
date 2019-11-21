#ifndef SSLERRORDIALOG_H
#define SSLERRORDIALOG_H

#include <QDialog>
#include <QSslError>


namespace Ui {
class SSLErrorDialog;
}

class SSLErrorDialog : public QDialog
{
	Q_OBJECT

public:
    explicit SSLErrorDialog(QWidget *parent = 0);
	~SSLErrorDialog();

	void displayErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain);
private:
	Ui::SSLErrorDialog *ui;
};

#endif // SSLERRORDIALOG_H
