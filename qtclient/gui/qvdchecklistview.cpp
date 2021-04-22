#include "qvdchecklistview.h"
#include <QProcessEnvironment>
#include <QSettings>
#include <QtDebug>

QSettings settings("Qindel","QVD Client");
QString sKey= "envvar";
auto env = QProcessEnvironment::systemEnvironment();

QVDListModel::QVDListModel(QObject *parent):
    QStringListModel(parent){
}

QVDListModel::QVDListModel(const QStringList &strings, QObject *parent):
    QStringListModel(strings, parent){
}

Qt::ItemFlags QVDListModel::flags (const QModelIndex & index) const {
    Qt::ItemFlags defaultFlags = QStringListModel::flags(index);
    if (index.isValid()){
        return defaultFlags | Qt::ItemIsUserCheckable;
    }
    return defaultFlags;
}

QVariant QVDListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();
    if(role == Qt::CheckStateRole)
        return checkedItems.contains(index) ?
                    Qt::Checked : Qt::Unchecked;
    else if(role == Qt::BackgroundRole)
        return checkedItems.contains(index) ?
                    QColor("#ffffb2") : QColor("#ffffff");
    return QStringListModel::data(index, role);
}

bool QVDListModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!index.isValid() || role != Qt::CheckStateRole)
        return false;
    if(value == Qt::Checked)
        checkedItems.insert(index);
    else
        checkedItems.remove(index);
    emit dataChanged(index, index);
    emit qvd_saveParameters();
    return true;
}

void QVDListModel::qvd_saveParameters(){
    QString sAuth, sText = "";
    QStringList list;
    int p=0;
    QString lEnvVar = "";
    foreach(QPersistentModelIndex index, checkedItems){
        lEnvVar = lEnvVar.trimmed() + " " + index.data().toString();
    }
    list = lEnvVar.trimmed().split(" ");
    QStringListIterator e(list);
    settings.beginGroup("Params");
    settings.beginWriteArray("QVDEnvVar");
    while (e.hasNext()) {
        settings.setArrayIndex(p);
        settings.setValue(sKey, e.next());
        p++;
    }
    settings.endArray();
    settings.endGroup();
}
