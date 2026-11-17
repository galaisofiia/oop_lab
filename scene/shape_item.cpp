#include "shape_item.h"
#include <QStyleOption>
#include <QPainter>
#include <QPolygonF>

ShapeItem::ShapeItem(ShapeType type, qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_type(type), m_width(width), m_height(height), m_dbId(-1) {
    setFlag(QGraphicsItem::ItemIsSelectable); // выделение фигуры кликом мыши
    setFlag(QGraphicsItem::ItemIsMovable); // перетаскивание фигуры по сцене
}

void ShapeItem::setType(ShapeType type) {
    m_type = type;
    update(); // перерисовка объекта
}

ShapeItem::ShapeType ShapeItem::getType() const {
    return m_type;
}

void ShapeItem::setDbId(int id) {
    m_dbId = id;
}

int ShapeItem::getDbId() const {
    return m_dbId;
}

// нахождение точных границ фигуры (для отрисовки и обработки кликов мыши)
QRectF ShapeItem::boundingRect() const {
    // прямоугольник
    return QRectF(-m_width / 2, -m_height / 2, m_width, m_height);
}

void ShapeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    // настраиваем контур (цвет черный, толщина 2 пикселя)
    painter->setPen(QPen(Qt::black, 2));
    // проверяем, выделена ли фигура 
    // (установлен ли флаг выделения внутри объекта option)
    if (option->state & QStyle::State_Selected) {
        painter->setBrush(Qt::lightGray);
    } else {
        painter->setBrush(Qt::white);
    }

    // отрисовка фигуры в зависимости от типа
    if (m_type == Rectangle) {
        painter->drawRect(boundingRect());
    } else if (m_type == Ellipse) {
        painter->drawEllipse(boundingRect());
    } else if (m_type == Polygon) {
        QPolygonF polygon;
        polygon << QPointF(0, -m_height / 2)
                << QPointF(m_width / 2, m_height / 2)
                << QPointF(-m_width / 2, m_height / 2);
        painter->drawPolygon(polygon);
    }
}