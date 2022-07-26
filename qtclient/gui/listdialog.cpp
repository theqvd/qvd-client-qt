#include "listdialog.h"
#include "ui_listdialog.h"
#include <QPushButton>
#include "QDebug"

#include <qvdchecklistview.h>
#include <QStringListModel>

struct QVDCert{
    QString PEM;
    qint32 Accept;
};

int VMNumber;
QStringList m_list_vm;
QVDListModel m_list_vm_model;
QTreeWidgetItem *vmlist_item;


ListDialog::ListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListDialog)
{
    ui->setupUi(this);
    ui->VMList->setHeaderLabels(QStringList({"ID", "Name"}));
    ui->VMList->setHeaderHidden(true);

    connect(ui->VMList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(IsItemClicked()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),this, SLOT(connect_vm()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),this, SLOT(cancel_connect()));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Connect"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

}

void ListDialog::DisplayVMs(const QList<QVDClient::VMInfo> &vmlist)
{
    ui->VMList->clear();
    bool first = true;


    for(auto vm : vmlist ) {
        QTreeWidgetItem *vmlist_item = new QTreeWidgetItem( ui->VMList );
        vmlist_item->setExpanded(true);
        vmlist_item->setData(0, 0, vm.id);
        vmlist_item->setText(1, vm.name);
        if ( first || vm.state == QVDClient::VMState::Running) {
            VMNumber =vm.id;
            vmlist_item->setSelected(true);

            first = false;
        }
    }
}

void ListDialog::IsItemClicked()
{
    QList<QTreeWidgetItem *> itemList;
    itemList = ui->VMList->selectedItems();
    foreach(QTreeWidgetItem *item, itemList)
    {
       VMNumber = item->text(0).toInt();
       qDebug() << VMNumber;
    }
}

void ListDialog::connect_vm()
{
    qDebug() << "(connect_vm) Conectar a la vm seleccionada";
    setResult(VMNumber);
    close();
}


void ListDialog::cancel_connect()
{
    qDebug() << "(cancel_connect) Cancelar conexión";
    setResult(0);
    close();
}

ListDialog::~ListDialog()
{
    delete ui;
}

#include "moc_listdialog.cpp"
