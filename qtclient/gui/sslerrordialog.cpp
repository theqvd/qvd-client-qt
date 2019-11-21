#include "sslerrordialog.h"
#include "ui_sslerrordialog.h"
#include <QPushButton>
#include "QDebug"
#include "QSettings"

SSLErrorDialog::SSLErrorDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SSLErrorDialog)
{
    ui->setupUi(this);

    //Opciones button
    connect(ui->aceptar, SIGNAL(clicked()),this, SLOT(close()));

    ui->errorTree->setHeaderLabels(QStringList({"Severity", "Error"}));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Accept temporarily"));
    ui->buttonBox->button(QDialogButtonBox::Save)->setText("Accept permanently");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

}

SSLErrorDialog::~SSLErrorDialog()
{
	delete ui;
}

void SSLErrorDialog::displayErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain)
{
	ui->errorTree->clear();

	QMap<QByteArray, QTreeWidgetItem*> cert_list;

	for(auto cert : cert_chain ) {
		QTreeWidgetItem *cert_item = new QTreeWidgetItem( ui->errorTree );
		cert_item->setExpanded(true);
		cert_item->setText(0, cert.subjectInfo(QSslCertificate::CommonName)[0]);
		cert_list[cert.digest()] = cert_item;

        cert.toPem();
        ui->certDescription->setPlainText(cert.toText());

	}

    for(auto error : errors ) {

        QTreeWidgetItem *root = cert_list[ error.certificate().digest( )];
		QTreeWidgetItem *e = new QTreeWidgetItem(root);

        e->setText(0, "Medium");
        e->setText(1, error.errorString());

	}
}
