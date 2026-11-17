#ifndef CUSTOM_SCENE_H
#define CUSTOM_SCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "shape_item.h"

class CustomScene : public QGraphicsScene {
    Q_OBJECT // обязательный макрос для компиляции сцены
public:
    explicit CustomScene(QObject *parent = nullptr);

    void setAddingShape(ShapeItem::ShapeType type);
    void setConnectingMode(bool enable);

signals:
    void shapeAdded(int dbId, ShapeItem::ShapeType type);
    void connectionAdded(int id1, int id2);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    ShapeItem::ShapeType m_currentShapeType;
    bool m_isAddingShape;
    bool m_isConnecting;
    ShapeItem *m_selectedItemForConnection;
    QGraphicsLineItem *m_tempLine;
};

#endif // CUSTOM_SCENE_H