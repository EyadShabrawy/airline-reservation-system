#include "customsqlquerymodel.h"

CustomSqlQueryModel::CustomSqlQueryModel(QObject* parent)
    : QSqlQueryModel(parent)
{
}

QVariant CustomSqlQueryModel::data(const QModelIndex& index, int role) const
{
    if (index.column() == columnCount() - 1 && role == Qt::DisplayRole)
        // Display "Book" text in the last column
        return "Book this flight";

    return QSqlQueryModel::data(index, role);
}

int CustomSqlQueryModel::columnCount(const QModelIndex& parent) const
{
    // Add an extra column for the "Book" button
    return QSqlQueryModel::columnCount(parent) + 1;
}

QVariant CustomSqlQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QSqlQueryModel::headerData(section, orientation, role);

    switch (section) {
    case 0:
        return "Flight Number";
    case 1:
        return "Departure Airport";
    case 2:
        return "Arrival Airport";
    case 3:
        return "Flight date";
    case 4:
        return "Flight duration";
    case 5:
        return "Number of seats";
    case 6:
        return "Price";
    case 7:
        return " ";
    default:
        return QVariant();
    }
}