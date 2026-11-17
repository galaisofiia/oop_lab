#include "mainwindow.h"
#include "delegates/shape_delegate.h"
#include "db/database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent) // конструктор
    : QMainWindow(parent), currentShapeType(0) {
    setupUi(); 
    setupModel(); // настройка связи с бд
}

// создание визуального интерфейса
void MainWindow::setupUi() {
    // основной виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget); // устнавливаем как главное окно

    scene = new CustomScene(this); // создаанем сцену
    view = new QGraphicsView(scene); // делаем сцену видимой

    tableView = new QTableView; // виджет таблицы
    // при клике на ячейку выделается весь столбец
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // кнопки интерфейса
    QPushButton *btnRect = new QPushButton("Добавить прямоугольник");
    QPushButton *btnEllipse = new QPushButton("Добавить эллипс");
    QPushButton *btnPolygon = new QPushButton("Добавить многоугольник");
    QPushButton *btnConnect = new QPushButton("Режим связи");
    QPushButton *btnDelete = new QPushButton("Удалить выбранное");

    filterIdEdit = new QLineEdit; // текстовое поле для ввода id фигуры
    filterIdEdit->setPlaceholderText("id фигуры"); // подсказка
    filterTypeCombo = new QComboBox; // выпадающий список
    filterTypeCombo->addItems({"Все", "прямоугольник", "эллипс", "многоугольник"});
    filterCountEdit = new QLineEdit;
    filterCountEdit->setPlaceholderText("количество");

    QPushButton *btnApplyFilter = new QPushButton("Применить фильтр");
    QPushButton *btnResetFilter = new QPushButton("Сбросить фильтр");

    // горизонтальный компоновщик для верхнего ряда кнопок действий
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(btnRect);
    topLayout->addWidget(btnEllipse);
    topLayout->addWidget(btnPolygon);
    topLayout->addWidget(btnConnect);
    topLayout->addWidget(btnDelete);

    // компоновщик для строки фильтров
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("id:"));
    filterLayout->addWidget(filterIdEdit);
    filterLayout->addWidget(new QLabel("Тип:"));
    filterLayout->addWidget(filterTypeCombo);
    filterLayout->addWidget(new QLabel("Кол-во:"));
    filterLayout->addWidget(filterCountEdit);
    filterLayout->addWidget(btnApplyFilter);
    filterLayout->addWidget(btnResetFilter);

    // вертикальный компоновщик
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(filterLayout);
    // просмотр сцены и таблицы на главном виджете
    mainLayout->addWidget(view, 1);
    mainLayout->addWidget(tableView, 1);

    // связь сигнала клика с лямбда-функциями, совершающие необходитмые приготовления
    connect(btnRect, &QPushButton::clicked, [this]() { currentShapeType = 0; onAddShape(); });
    connect(btnEllipse, &QPushButton::clicked, [this]() { currentShapeType = 1; onAddShape(); });
    connect(btnPolygon, &QPushButton::clicked, [this]() { currentShapeType = 2; onAddShape(); });
    connect(btnConnect, &QPushButton::clicked, [this]() { scene->setConnectingMode(true); });
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(btnApplyFilter, &QPushButton::clicked, this, &MainWindow::onFilterChanged);
    // функция сброса
    connect(btnResetFilter, &QPushButton::clicked, [this]() {
        filterIdEdit->clear();
        filterTypeCombo->setCurrentIndex(0);
        filterCountEdit->clear();
        model->setFilter("");
        model->select();
    });

    // обработка сигнала создания фигуры
    connect(scene, &CustomScene::shapeAdded, this, [this](int dbId, ShapeItem::ShapeType type) {
        QSqlQuery query; // объект для выполнения sql-запроса
        // подготавливаем запрос на вставку новой записи
        query.prepare("insert into shapes (type, total_count) values (:type, 0)");
        // прикрепляем к запросу числовое значение типа фигуры
        query.bindValue(":type", static_cast<int>(type));
        query.exec(); // выполняем запрос на вставку
        int newId = query.lastInsertId().toInt(); // генерируем id

        // подготавливаем второй объект запроса для операции обновления
        QSqlQuery updateQuery;
        // подготавливаем запрос с пересчетом общего количества фигур во всех строках данного типа в таблице
        updateQuery.prepare("update shapes set total_count = (select count(*) from shapes where type = :type) where type = :type");
        updateQuery.bindValue(":type", static_cast<int>(type));
        updateQuery.exec(); // выполняем запрос на подготовку

        model->select(); // обновляяем отображение таблицы
        // перебираем все графические объекты на сцене
        for (QGraphicsItem *item : scene->items()) {
            // проверяем, что объект - типа фигуры
            ShapeItem *shape = dynamic_cast<ShapeItem*>(item);
            // проверяем, что у объекта еще нет id
            if (shape && shape->getDbId() == -1) {
                shape->setDbId(newId); // присваиваем id
                break;
            }
        }
    });

    // обработка сигнала создания связи
    connect(scene, &CustomScene::connectionAdded, this, [this](int id1, int id2) {
        QSqlQuery query; //  объект запроса
        // подготовка запроса на вставку пары id в таблицу связей
        query.prepare("insert into connections (shape_id_1, shape_id_2) values (:id1, :id2)");
        // привязка id к параметрам запроса
        query.bindValue(":id1", id1);
        query.bindValue(":id2", id2);
        query.exec(); // выполняем запрос на сохранение связи в бд
    });
}

// связь между бд и таблицей на экране
void MainWindow::setupModel() {
    // модель данных, автоматически обновляющая таблицу данных
    model = new QSqlTableModel(this, Database::getDb());
    model->setTable("shapes"); // указываем какую таблицу мы хотим видеть
    // все изменения применяем к дб только вручную
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select(); // отображаем текущую таблицу

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Тип");
    model->setHeaderData(2, Qt::Horizontal, "Количество");

    // привязываем настроенную модель к таблице
    tableView->setModel(model);
    // назначаем нашего делегата для отрисовки ячеек
    tableView->setItemDelegate(new ShapeDelegate(1, this));
}

// передача выбранного типа фигуры на сцену
void MainWindow::onAddShape() {
    scene->setAddingShape(static_cast<ShapeItem::ShapeType>(currentShapeType));
}

// удаление выбранной фигуры
void MainWindow::onDelete() {
    // запрос всех выделенных строк перед их удалением
    QModelIndexList selected = tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    // проход по выыделенным строкам
    for (const QModelIndex &index : selected) {
        int id = index.sibling(index.row(), 0).data().toInt();
        // перебор всех графических объектов на сцене, чтобы найти
        // с нашим id
        for (QGraphicsItem *item : scene->items()) {
            ShapeItem *shape = dynamic_cast<ShapeItem*>(item);
            if (shape && shape->getDbId() == id) {
                // сохраняем координаты фигуры перед удалением
                QPointF shapePos = shape->pos();
                // ищем линии связи с нашим объектом
                for (QGraphicsItem *sceneItem : scene->items()) {
                    // проверяем, является ли объект линией
                    QGraphicsLineItem *line = dynamic_cast<QGraphicsLineItem*>(sceneItem);
                    if (line) {
                        // получаем координаты концов линии
                        QLineF l = line->line();
                        // если наша фигура связана этой линией - удаляем
                        if (l.p1() == shapePos || l.p2() == shapePos) {
                            scene->removeItem(line);
                            delete line;
                        }
                    }
                } // удаляем саму фигуру
                scene->removeItem(shape);
                delete shape;
                break;
            }
        } // помечаем текущую строку для удаления в модели данных
        model->removeRow(index.row());
    } // подтверждаем все удаления
    model->submitAll();

    // пересчитываем total_count для оставшихся типов
    QSqlQuery recountQuery; // объект для запроса обновления
    // выполняем запрос, который обновляет общее кол-во фигур каждого типа во всех строках
    recountQuery.exec("update shapes set total_count = (select count(*) from shapes s2 where s2.type = shapes.type)");
    model->select(); // обновляем отображение таблицы
}

// метод фильтрации
void MainWindow::onFilterChanged() {
    QString filter; // строка для будущих sql-запросов

    if (!filterIdEdit->text().isEmpty()) {
        // добавляем в запрос введеный id
        filter += QString("id = %1").arg(filterIdEdit->text());
    }

    // получаем индекс выбранного элемента в выпадающем списке
    int typeIndex = filterTypeCombo->currentIndex();
    if (typeIndex > 0) { // 0 - пункт "все"
        if (!filter.isEmpty()) filter += " and ";
        filter += QString("type = %1").arg(typeIndex - 1);
    }

    if (!filterCountEdit->text().isEmpty()) {
        if (!filter.isEmpty()) filter += " and ";
        filter += QString("total_count = %1").arg(filterCountEdit->text());
    }

    model->setFilter(filter); // передаем собранную строку условий в модель данных
    model->select(); // выполняем запрос и обновляем отображение таблицы
}