#include "totpenrollment.h"
#include "ui_totpenrollment.h"
#include "qvdclient.h"
#include <QDebug>


TotpEnrollment::TotpEnrollment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TotpEnrollment)
{
    ui->setupUi(this);
}

TotpEnrollment::~TotpEnrollment()
{
    delete ui;
}

void TotpEnrollment::setData(const QVDClient::SecondFactorEnrollmentData &data)
{
    qDebug() << "Setting up TOTP enrollment";
    ui->totpSecret->setText(data.Secret);

    if ( m_qr_pixmap.loadFromData(data.QrPicture) ) {
        qDebug() << "Loaded QR code image, " << m_qr_pixmap.width() << "x" << m_qr_pixmap.height();
        ui->qrCodeLabel->setPixmap(m_qr_pixmap.scaled(ui->qrCodeLabel->width(), ui->qrCodeLabel->height(), Qt::KeepAspectRatio, Qt::FastTransformation));
    } else {
        qCritical() << "Failed to decode QR code image";
    }

    if (!data.helpMessage.isEmpty()) {
        ui->adminMessageLabel->setText(data.helpMessage);
    } else {
        ui->supportLabel->setVisible(false);
        ui->adminMessageLabel->setVisible(false);
    }
}

void TotpEnrollment::windowResized(QResizeEvent *ev)
{
}


void TotpEnrollment::resizeEvent(QResizeEvent *ev)
{
    ui->qrCodeLabel->setPixmap(m_qr_pixmap.scaled(ui->qrCodeLabel->width(), ui->qrCodeLabel->height(), Qt::KeepAspectRatio, Qt::FastTransformation));
}
