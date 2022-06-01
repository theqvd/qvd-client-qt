#ifndef TOTPENROLLMENT_H
#define TOTPENROLLMENT_H

#include <QDialog>
#include <QPixmap>
#include <QResizeEvent>
#include "qvdclient.h"

namespace Ui {
class TotpEnrollment;
}

class TotpEnrollment : public QDialog
{
    Q_OBJECT

public:
    explicit TotpEnrollment(QWidget *parent = nullptr);
    ~TotpEnrollment();

    void setData(const QVDClient::SecondFactorEnrollmentData &data);
    void windowResized(QResizeEvent *ev);

    virtual void resizeEvent(QResizeEvent *ev);
private:
    Ui::TotpEnrollment *ui;
    QPixmap m_qr_pixmap;

};

#endif // TOTPENROLLMENT_H
