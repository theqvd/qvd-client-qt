#ifndef LISTDIALOG_H
#define LISTDIALOG_H

#include <QDialog>
#include <qvdclient.h>
#include "QSettings"

#include <QStringList>
#include <QStringListModel>
#include <QTreeWidget>

namespace Ui {
class ListDialog;
}

class ListDialog : public QDialog
{
	Q_OBJECT

public:
    explicit ListDialog(QWidget *parent = 0);
    ~ListDialog();

    void DisplayVMs(const QList<QVDClient::VMInfo> &vmlist);

public slots:
    void IsItemClicked();


private slots:
    void connect_vm();
    void cancel_connect();

private:
    Ui::ListDialog *ui;
    QStringListModel *model;
};

#endif // LISTDIALOG_H
