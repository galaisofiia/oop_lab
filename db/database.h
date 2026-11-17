#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>

class Database {
    static QSqlDatabase db;

public:
    static bool connectToDb();
    static QSqlDatabase getDb();
};

#endif // DATABASE_H