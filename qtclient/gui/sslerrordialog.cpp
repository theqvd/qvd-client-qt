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

    QSettings settings_cert;
    settings_cert.beginGroup("SSL");

    ui->errorTree->setHeaderLabels(QStringList({"Severity", "Error"}));

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),this, SLOT(accept_ssl()));
    connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()),this, SLOT(accept_save_ssl()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),this, SLOT(reject_ssl()));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Accept temporarily"));
    ui->buttonBox->button(QDialogButtonBox::Save)->setText("Accept permanently");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

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
        //cert.toPem();
        ui->certDescription->setPlainText(cert.toText());

    }

    for(auto error : errors ) {

        QTreeWidgetItem *root = cert_list[ error.certificate().digest( )];
        QTreeWidgetItem *e = new QTreeWidgetItem(root);

        e->setText(0, "Medium");
        e->setText(1, error.errorString());

    }
}

void SSLErrorDialog::accept_ssl()
{
    qDebug() << "Accepting settings certificate once";
    close();
}

void SSLErrorDialog::accept_save_ssl()
{
    qDebug() << "Saving settings certificate";
    // if (settings_cert.value("PEM", "").toString() == "")
    //settings_cert.setValue("PEM", cert.toPem());
    close();
}

void SSLErrorDialog::reject_ssl()
{
    qDebug() << "Reject settings certificate";
    close();
}

SSLErrorDialog::~SSLErrorDialog()
{
    delete ui;
}
