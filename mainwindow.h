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
    void copyUserNameButton();
    void copyPasswordButton();
    void generatePassword();
    void saveTableData();
    bool loadTableData();

private:
    QTableWidget *table;
    QPushButton *addRowButton;
    QString filePath = "password.txt"; 
};

#endif 
