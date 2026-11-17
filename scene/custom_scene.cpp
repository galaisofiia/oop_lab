#include "custom_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <QPen>

// конструктор
CustomScene::CustomScene(QObject *parent)
    : QGraphicsScene(parent), m_isAddingShape(false), m_isConnecting(false),
      m_selectedItemForConnection(nullptr), m_tempLine(nullptr) {
}

// включение режима добавления фигуры
void CustomScene::setAddingShape(ShapeItem::ShapeType type) {
    m_currentShapeType = type;
    m_isAddingShape = true;
    m_isConnecting = false;
}

// включение режима создания связей между фигурами
void CustomScene::setConnectingMode(bool enable) {
    m_isConnecting = enable;
    m_isAddingShape = false;
    if (!enable) {
        m_selectedItemForConnection = nullptr;
    }
}

// обработка клика мыши
void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // режим добавления фигуры
    if (m_isAddingShape) {
        QPointF pos = event->scenePos(); // координаты точки клика
        // проверяем, что позиция валидна
        if (!pos.isNull()) {
            ShapeItem *item = new ShapeItem(m_currentShapeType, 60, 40);
            item->setPos(pos);
            addItem(item); // размещение фигуры на сцене
            emit shapeAdded(0, m_currentShapeType); // испускаем сигнал
        }
        m_isAddingShape = false; // выключение режима

    // режим добавления связей
    } else if (m_isConnecting) {
        // проверка, на какой предмет нажали
        // QTransform - без учета преобразований объекта
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        // безопасное преобразование типа объекта, на который нажали
        // (фигура или пустота)
        ShapeItem *shape = dynamic_cast<ShapeItem*>(item);
        if (shape) { // кликнули на фигуру
            if (!m_selectedItemForConnection) { // выбираем первую фигуру для связи
                m_selectedItemForConnection = shape;
                shape->setSelected(true); // выделяем фигуру
            } else { // выбираем вторую фигуру для связи
                // проверка, что фигура не одна и та же
                if (m_selectedItemForConnection != shape) {
                    // создаем линию
                    QLineF line(m_selectedItemForConnection->pos(), shape->pos());
                    addLine(line, QPen(Qt::black, 2)); // рисуем линию
                    // испускаем сигнал для записи в бд связи между двумя фигурами
                    emit connectionAdded(m_selectedItemForConnection->getDbId(), shape->getDbId());
                }
                m_selectedItemForConnection->setSelected(false); // снимаем выделение
                m_selectedItemForConnection = nullptr; // очищаем переменную
            }
        }
    } else { // обычный режим взаимодействия
        QGraphicsScene::mousePressEvent(event);
    }
}

// обработка движения мыши с зажатой кнопкой
void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    // режим связи, когда первая фигура уже выбрана
    if (m_isConnecting && m_selectedItemForConnection) {
        if (m_tempLine) { // если временная линия уже существует, удаляем  ее
            removeItem(m_tempLine);
            delete m_tempLine;
        }
        // создаем линию от выбраной фигуры до курсора
        QLineF line(m_selectedItemForConnection->pos(), event->scenePos());
        // добавляем временную линию (пунктир)
        m_tempLine = addLine(line, QPen(Qt::gray, 2, Qt::DashLine));
    } else { // обычный режим взаимодействия
        QGraphicsScene::mouseMoveEvent(event);
    }
}

// обработка отпускания кнопки мыши
void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_tempLine) {
        removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}
