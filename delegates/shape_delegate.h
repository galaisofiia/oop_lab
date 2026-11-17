#ifndef SHAPE_DELEGATE_H
#define SHAPE_DELEGATE_H

#include <QStyledItemDelegate>

// делегат, отвечающий за отрисовку ячеек таблицы
class ShapeDelegate : public QStyledItemDelegate {
public:
    explicit ShapeDelegate(int typeColumn, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    int m_typeColumn;

    void drawShape(QPainter *painter, const QRect &rect, int type) const;
};

#endif // SHAPE_DELEGATE_H