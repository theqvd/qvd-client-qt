#include "sslerrordialog.h"
#include "ui_sslerrordialog.h"
#include <QPushButton>
#include "QDebug"

struct QVDCert{
    QString PEM;
    qint32 Accept;
};

QList<QVDCert> qvdCerts;

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
    ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("Accept permanently"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

}

void SSLErrorDialog::displayErrors(const QList<QSslError> &errors, const QList<QSslCertificate> &cert_chain)
{
    ui->errorTree->clear();

    QMap<QByteArray, QTreeWidgetItem*> cert_list;

    for(auto cert : cert_chain )
    {

            QTreeWidgetItem *cert_item = new QTreeWidgetItem( ui->errorTree );
            cert_item->setExpanded(true);
            cert_item->setText(0, cert.subjectInfo(QSslCertificate::CommonName)[0]);
            cert_list[cert.digest()] = cert_item;

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
    qDebug() << "(accept_ssl) Accepting settings certificate once";
    setResult(1);
    close();
}

void SSLErrorDialog::accept_save_ssl()
{
    qDebug() << "(accept_save_ssl) Saving settings certificate";
    setResult(2);
    close();
}

void SSLErrorDialog::reject_ssl()
{
    qDebug() << "(reject_ssl) Reject settings certificate";
    setResult(0);
    close();
}

SSLErrorDialog::~SSLErrorDialog()
{
    delete ui;
}
