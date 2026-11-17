#ifndef SHAPE_ITEM_H
#define SHAPE_ITEM_H

#include <QGraphicsItem>

class ShapeItem : public QGraphicsItem {
public:
    enum ShapeType {
        Rectangle,
        Ellipse,
        Polygon
    };

    ShapeItem(ShapeType type, qreal width, qreal height, QGraphicsItem *parent = nullptr);

    void setType(ShapeType type);
    ShapeType getType() const;
    void setDbId(int id);
    int getDbId() const;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    ShapeType m_type;
    qreal m_width;
    qreal m_height;
    int m_dbId;
};

#endif // SHAPE_ITEM_H