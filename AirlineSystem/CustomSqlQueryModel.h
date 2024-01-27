#pragma once
#include <qsqlquerymodel.h>
#include <QSqlQueryModel>
class CustomSqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit CustomSqlQueryModel(QObject* parent = nullptr);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

