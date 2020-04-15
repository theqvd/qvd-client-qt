#ifndef QVDCHECKLISTVIEW_H
#define QVDCHECKLISTVIEW_H

#include <QStringListModel>
#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QSet>

class QVDListModel : public QStringListModel
{
public:
    QVDListModel(QObject* parent = 0);
    QVDListModel(const QStringList & strings, QObject* parent = 0);
    Qt::ItemFlags flags (const QModelIndex& index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role);
    void qvd_saveParameters();
private:
    QSet<QPersistentModelIndex> checkedItems;
};

#endif // QVDCHECKLISTVIEW_H
