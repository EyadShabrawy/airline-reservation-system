#include "BookButtonDelegate.h"
#include <QApplication>
#include <QMouseEvent>

BookButtonDelegate::BookButtonDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void BookButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == index.model()->columnCount() - 1) {
        QStyleOptionButton button;
        button.rect = option.rect.adjusted(4, 4, -4, -4);
        button.text = "Book";
        button.state = QStyle::State_Enabled;

        QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

bool BookButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonRelease && index.column() == index.model()->columnCount() - 1) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            emit bookButtonClicked(index);
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}