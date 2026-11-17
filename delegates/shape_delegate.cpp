#include "shape_delegate.h"
#include <QPainter>
#include <QModelIndex>
#include <QApplication>
#include <QStyle>

// конструктор
ShapeDelegate::ShapeDelegate(int typeColumn, QObject *parent)
    : QStyledItemDelegate(parent), m_typeColumn(typeColumn) {
}

// отрисовка ячеек таблицы и их содержимого
void ShapeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // колонка "тип" 
    if (index.column() == 1) {
        int type = index.data().toInt();
        QString typeText;
        if (type == 0) typeText = "прямоугольник"; 
        else if (type == 1) typeText = "эллипс";
        else if (type == 2) typeText = "многоугольник";
        
        QStyleOptionViewItem opt = option; // копия настроек стиля ячейки
        initStyleOption(&opt, index); // заполняет opt данными из index
        opt.text = typeText; // переписываем текст чтобы выводилось слово
        
        // рисуем ячейку
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
        return;
    }
    
    // колонка "количество"
    if (index.column() == 2) {
        int count = index.data().toInt(); // получаем текущее количество
        // получаем фигуру, которую надо отрисовывать
        int type = index.sibling(index.row(), m_typeColumn).data().toInt();

        QStyleOptionViewItem opt = option; // копия настроек стиля ячейки
        initStyleOption(&opt, index);
        opt.text.clear(); // удаляем текст
        
        // рисуем ячейку (пока без текста и фигур)
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

        int iconsToDraw = 0;
        if (count >= 1 && count <= 3) {
            iconsToDraw = 1;
        } else if (count >= 4 && count <= 10) {
            iconsToDraw = 2;
        } else if (count > 10) {
            iconsToDraw = 3;
        }

        QRect cellRect = option.rect; // границы текущей ячейки
        int iconSize = qMin(cellRect.width(), cellRect.height()) / 2;
        int startX = cellRect.left() + (cellRect.width() - iconsToDraw * iconSize) / 2;
        int startY = cellRect.top() + (cellRect.height() - iconSize) / 2;

        for (int i = 0; i < iconsToDraw; ++i) {
            QRect iconRect(startX + i * iconSize, startY, iconSize, iconSize);
            drawShape(painter, iconRect, type);
        }
        return;
    }
    
    // остальные колонки
    QStyledItemDelegate::paint(painter, option, index);
}

// отрисовка фигуры
void ShapeDelegate::drawShape(QPainter *painter, const QRect &rect, int type) const {
    painter->setPen(QPen(Qt::black, 1)); // обводка
    painter->setBrush(Qt::lightGray); // заливка

    if (type == 0) {
        painter->drawRect(rect);
    } else if (type == 1) {
        painter->drawEllipse(rect);
    } else if (type == 2) {
        QPolygonF polygon;
        polygon << QPointF(rect.center().x(), rect.top())
                << QPointF(rect.right(), rect.bottom())
                << QPointF(rect.left(), rect.bottom());
        painter->drawPolygon(polygon);
    }
}