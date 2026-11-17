#include "database.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

QSqlDatabase Database::db;

// подключение к файлу базы данных (далее - бд)
bool Database::connectToDb() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("shapes.db");

    if (!db.open()) { // не удалось открыть файл бд
        qDebug() << "db open error:" << db.lastError().text();
        return false;
    }

    // создание таблиц
    QSqlQuery query; // отправка запросов в бд
    query.exec("create table if not exists shapes ("
               "id integer primary key autoincrement, "
               "type text not null, " // тип фигуры
               "total_count integer not null default 0)");

    // таблица связей
    query.exec("create table if not exists connections ("
               "id integer primary key autoincrement, "
               "shape_id_1 integer not null, " // 1я фигура из пары
               "shape_id_2 integer not null, " // 2я фигура из пары

               // связь с id из таблицы shapes
               // on delete cascade - автоматическое удаление связей
               "foreign key(shape_id_1) references shapes(id) on delete cascade, "
               "foreign key(shape_id_2) references shapes(id) on delete cascade)");

    return true;
}

QSqlDatabase Database::getDb() {
    return db;
}