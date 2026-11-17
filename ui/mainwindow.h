#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QTableView>
#include <QSqlTableModel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include "scene/custom_scene.h"

// окно
class MainWindow : public QMainWindow {
    Q_OBJECT // макрос для сигналов и слотов
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onAddShape();
    void onDelete();
    void onFilterChanged();

private:
    void setupUi();
    void setupModel();

    CustomScene *scene;
    QGraphicsView *view;
    QTableView *tableView;
    QSqlTableModel *model;
    QComboBox *filterTypeCombo;
    QLineEdit *filterIdEdit;
    QLineEdit *filterCountEdit;

    int currentShapeType;
};

#endif // MAINWINDOW_H