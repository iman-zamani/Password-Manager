#include "mainwindow.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QHeaderView>
#include <QClipboard>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // table of passwords setup
    table = new QTableWidget(this);
    table->setColumnCount(7);
    QStringList tableHeader{"Website", "User Name", "Password", "Remove", " ", " "," "};
    table->setHorizontalHeaderLabels(tableHeader);
    table->setRowCount(0);
    table->setEditTriggers(QAbstractItemView::DoubleClicked);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    loadTableData(); // load data on start
    //------------------------------------------------------------------------------------
    // UI setup
    addRowButton = new QPushButton("Add Row", this);
    connect(addRowButton, &QPushButton::clicked, this, &MainWindow::addRow);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(table);
    layout->addWidget(addRowButton);
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);
    QScreen *screen = QApplication::screens().at(0);
    QRect screenSize = screen->availableGeometry();
    resize(screenSize.width() / 2, screenSize.height() / 2);
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Set the main window as the parent of the QMessageBox for the cancel option 
    QMessageBox msgBox(this); 
    msgBox.setWindowTitle("Save Data");
    msgBox.setText("Do you want to save the changes to the table?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    // center the pop up base on window 
    msgBox.move(this->geometry().center() - msgBox.rect().center());

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        saveTableData();
        event->accept(); // save and then close the app
    } else if (ret == QMessageBox::No) {
        event->accept();  // close the app 
    } else {
        event->ignore();  // keep the app open
    }
}


void MainWindow::saveTableData()
{
    // this function  will store the first 3 columns (website , user name and password)
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        for (int i = 0; i < table->rowCount(); ++i) {
            for (int j = 0; j < table->columnCount() - 3; ++j) { 
                if (j > 0) stream << ",";
                QTableWidgetItem *item = table->item(i, j);
                if (item)
                    stream << item->text();
            }
            stream << "\n";
        }
        file.close();
    }
}

bool MainWindow::loadTableData()
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList values = line.split(",");
        addRow();
        int lastRow = table->rowCount() - 1;
        for (int i = 0; i < values.size() && i < 3; ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(values.at(i));
            table->setItem(lastRow, i, item);
        }
    }
    file.close();
    return true;
}
void MainWindow::addRow()
{
    int rowCount = table->rowCount();
    table->insertRow(rowCount);
    QPushButton *removeButton = new QPushButton("Remove Row");
    removeButton->setStyleSheet("background-color: red;");
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeRow);
    table->setCellWidget(rowCount, 3, removeButton);

    QPushButton *copyUserNameButton = new QPushButton("Copy User Name");
    copyUserNameButton->setStyleSheet("background-color: darkblue; color: white;");
    connect(copyUserNameButton, &QPushButton::clicked, this, &MainWindow::copyUserNameButton);
    table->setCellWidget(rowCount, 4, copyUserNameButton);

    QPushButton *copyPasswordButton = new QPushButton("Copy Password");
    copyPasswordButton->setStyleSheet("background-color: darkblue; color: white;");
    connect(copyPasswordButton, &QPushButton::clicked, this, &MainWindow::copyPasswordButton);
    table->setCellWidget(rowCount, 5, copyPasswordButton);

    QPushButton *generatePassword = new QPushButton("Generate Password");
    generatePassword->setStyleSheet("background-color: green; color: white;");
    connect(generatePassword, &QPushButton::clicked, this, &MainWindow::generatePassword);
    table->setCellWidget(rowCount, 6, generatePassword);
}
void MainWindow::copyUserNameButton()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    int row = table->indexAt(button->pos()).row();
    if (row != -1) {
        QTableWidgetItem *item = table->item(row, 1);  // username is index 1 
        if (item) {
            QString text = item->text();
            QApplication::clipboard()->setText(text);
        }
    }
}

void MainWindow::copyPasswordButton()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    int row = table->indexAt(button->pos()).row();
    if (row != -1) {
        QTableWidgetItem *item = table->item(row, 2);  // password is index 2
        if (item) {
            QString text = item->text();
            QApplication::clipboard()->setText(text);
        }
    }
}
void MainWindow::generatePassword(){

}
void MainWindow::removeRow()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int row = table->indexAt(button->pos()).row();
    if (row != -1) {
        table->removeRow(row);
    }
}
