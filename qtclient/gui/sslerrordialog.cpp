#include "sslerrordialog.h"
#include "ui_sslerrordialog.h"
#include <QPushButton>
#include "QDebug"

SSLErrorDialog::SSLErrorDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::SSLErrorDialog)
{
    ui->setupUi(this);
    //settings_cert.beginGroup("SSL");
    settings_cert.beginWriteArray("SSL");

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
    qint8 i;

    struct QVDCert{
        QString PEM;
        qint8 Accept;
    };

    QList<QVDCert> qvdCerts;

    ui->errorTree->clear();

	QMap<QByteArray, QTreeWidgetItem*> cert_list;

    i=0;
    for(auto cert : cert_chain )
        {

            QTreeWidgetItem *cert_item = new QTreeWidgetItem( ui->errorTree );
            cert_item->setExpanded(true);
            cert_item->setText(0, cert.subjectInfo(QSslCertificate::CommonName)[0]);
            cert_list[cert.digest()] = cert_item;

            ui->certDescription->setPlainText(cert.toText());

            settings_cert.setArrayIndex(i);
            QVDCert qvdCert;
            qvdCert.PEM = cert.toPem();
            qvdCert.Accept = 0;
            qvdCerts.append(qvdCert);

            settings_cert.setArrayIndex(i);
            settings_cert.setValue("PEM", qvdCerts.at(i).PEM);
            settings_cert.setValue("Accept", qvdCerts.at(i).Accept);

            qDebug() << "----------------------------------------------------------------";
            qDebug() << " Values in PEM:" << settings_cert.value("PEM").toString();
            qDebug() << " Values in Accept:" << settings_cert.value("Accept").toString();
            qDebug() << "----------------------------------------------------------------";

            i = i + 1;
        }
        i = 0;
        settings_cert.endArray();

    for(auto error : errors ) {

        QTreeWidgetItem *root = cert_list[ error.certificate().digest( )];
        QTreeWidgetItem *e = new QTreeWidgetItem(root);

        e->setText(0, "Medium");
        e->setText(1, error.errorString());

    }
}

void SSLErrorDialog::accept_ssl()
{
    qDebug() << "(accept_ssl) Accepting settings certificate once";
    settings_cert.setValue("Accept", 1);
    close();
}

void SSLErrorDialog::accept_save_ssl()
{
    qDebug() << "(accept_save_ssl) Saving settings certificate";
    settings_cert.setValue("Accept", 2);
    close();
}

void SSLErrorDialog::reject_ssl()
{
    qDebug() << "(reject_ssl) Reject settings certificate";
    settings_cert.setValue("Accept", 0);
    close();
}

SSLErrorDialog::~SSLErrorDialog()
{
    delete ui;
}
