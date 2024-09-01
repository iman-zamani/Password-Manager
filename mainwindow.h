#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QCloseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(); 

protected:
    void closeEvent(QCloseEvent *event) override; 

private slots:
    void addRow();
    void removeRow();
    void copyCellB();
    void copyCellC();
    void saveTableData();
    bool loadTableData();

private:
    QTableWidget *table;
    QPushButton *addRowButton;
    QString filePath = "password.csv"; 
};

#endif 
