#include <QApplication>
#include "ui/mainwindow.h"
#include "db/database.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv); // главный объект qt

    if (!Database::connectToDb()) {
        // не удалось подключиться к бд
        return -1;
    }

    MainWindow window; // создание главного окна
    window.resize(1000, 800);
    window.show();

    return app.exec(); // цикл обработки событий
}