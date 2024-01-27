#pragma once
#include <qstyleditemdelegate.h>
#include <QStyledItemDelegate>
class BookButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit BookButtonDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

signals:
    void bookButtonClicked(const QModelIndex& index);
};

