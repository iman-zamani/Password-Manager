#include <iostream>
#include <string>
#include <cstring> 
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QHeaderView>
#include <QClipboard>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include "encrypt.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // table of passwords setup
    table = new QTableWidget(this);
    table->setColumnCount(7);
    QStringList tableHeader{"Website", "User Name", "Password", " ", " ", " "," "};
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
void setStringToSpaces(std::string& str) {
    for (char& c : str) {
        c = ' ';  
    }
}
void setQStringToSpaces(QString& qStr) {
    qStr.fill(' ');  
    qStr.clear();
    qStr.squeeze();
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
        saveTableData(event);
        event->accept(); // save and then close the app
    } else if (ret == QMessageBox::No) {
        event->accept();  // close the app 
    } else {
        event->ignore();  // keep the app open
    }
}

std::string MainWindow::getUserPassword() {
    QInputDialog inputDialog(this);
    inputDialog.setModal(true); 
    inputDialog.setLabelText(tr("Enter your password:"));
    inputDialog.setWindowTitle(tr("Authentication"));
    inputDialog.setTextEchoMode(QLineEdit::Password);

    QSize screenSize = this->screen()->size();  
    int width = screenSize.width() / 4;
    int height = screenSize.height() / 3;
    int x = (screenSize.width() - width) / 2;
    int y = (screenSize.height() - height) / 2;
    inputDialog.resize(width, height);
    inputDialog.move(x, y);

    bool ok = inputDialog.exec() == QDialog::Accepted;
    QString password = inputDialog.textValue();

    // attempt to clear memory
    std::string stdPassword;
    if (ok && !password.isEmpty()) {
        stdPassword = password.toStdString();
        // clear the QString password data
        setQStringToSpaces(password);
        return stdPassword;
    } else {
        QMessageBox::warning(this, tr("Input Required"),
                             tr("You must enter a password to proceed."));
        return ""; 
    }
    // clear stdPassword
    //std::memset(&stdPassword[0], 0, stdPassword.size());
    //stdPassword.clear();
    //setStringToSpaces(stdPassword);
    return stdPassword;
}
QString copyFileContent(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString(); 
    }

    QTextStream stream(&file);
    QString contents = stream.readAll();
    file.close(); 

    return contents;
}
void MainWindow::saveTableData(QCloseEvent *event) {
    QString copyFile = copyFileContent(filePath); // backup original file content in case of error
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Save Error"), tr("Unable to open file for saving. Please check your permissions or disk space."));
        return;
    }

    QTextStream stream(&file);
    QString longString;

    // iterate over each row and column to build a single long string
    for (int i = 0; i < table->rowCount(); ++i) {
        for (int j = 0; j < 3; ++j) { // Assuming you're only interested in the first three columns
            QTableWidgetItem *item = table->item(i, j);
            if (item) {
                longString += item->text();
                if (j < 2)
                    longString += ";"; // using ';' as a cell separator
            }
            else {
                longString += " ";
                if (j < 2)
                    longString += ";"; // using ';' as a cell separator
            }
        }
        if (i < table->rowCount() - 1)
            longString += "|"; // use '|' as a row separator
    }

    bool authenticated = false;
    std::string enteredPassword;
    for (int attempts = 0; attempts < 3; ++attempts) {
        enteredPassword = getUserPassword();
        if (enteredPassword.empty()) {
            QMessageBox::warning(this, tr("Authentication Required"), tr("You must enter a password to proceed."));
            continue;
        }

        if (checkPassword({reinterpret_cast<const unsigned char*>(enteredPassword.data()), enteredPassword.size()}, userPasswordHash, userSalt)) {
            authenticated = true;
            break;
        } else {
            QMessageBox::warning(this, tr("Authentication Failed"), tr("Incorrect password. Please try again."));
        }
    }

    if (!authenticated) {
        QMessageBox::warning(this, tr("Authentication Failed"), tr("Maximum retry attempts reached."));
        stream << copyFile; // moving the content back to file (because the write method deleted it)
        file.close();
        return;
    }

    std::string plainText = longString.toStdString();
    std::string encodedCiphertextString = EncryptString(plainText, enteredPassword);
    QString saveString = QString::fromStdString(encodedCiphertextString);
    stream << saveString;

    file.close();
    setStringToSpaces(plainText);
    setStringToSpaces(enteredPassword);
    setStringToSpaces(encodedCiphertextString);
    setQStringToSpaces(longString);

    QMessageBox::information(this, tr("Save Successful"), tr("Your data has been encrypted and saved."));
}


bool MainWindow::loadTableData() {
    std::string userPassword = getUserPassword();
    // save hash
    if (!userPassword.empty()) {
        userSalt = generateSalt();
        CryptoPP::SecByteBlock password(reinterpret_cast<const unsigned char*>(userPassword.data()), userPassword.size());
        userPasswordHash = savePassword(password, userSalt);
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream stream(&file);
     // read the entire file content into a single string
    QString encodedCiphertextQString = stream.readAll(); 
    file.close();
    std::string encodedCiphertextString = encodedCiphertextQString.toStdString();
    // decryption of the file
    std::string decodedCiphertextString = DecryptString(encodedCiphertextString,userPassword);
    QString longString = QString::fromStdString(decodedCiphertextString);
    // split the string into rows
    QStringList rows = longString.split("|");
    setQStringToSpaces(longString);

    for (QString &row : rows) {
        QStringList values = row.split(";");
        addRow();
        int lastRow = table->rowCount() - 1;
        for (int i = 0; i < values.size() && i < 3; ++i) {
         QTableWidgetItem *item = new QTableWidgetItem(values.at(i));

            setQStringToSpaces(values[i]);

            table->setItem(lastRow, i, item);
        }
        setQStringToSpaces(row);
    }

    setStringToSpaces(decodedCiphertextString);
    setStringToSpaces(userPassword);
    memset(&userPassword[0], 0, userPassword.size()); 
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
void MainWindow::generatePassword() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int row = table->indexAt(button->pos()).row();
    int passwordColumn = 2;  // password is index 2

    if (row != -1) {
        QTableWidgetItem *item = table->item(row, passwordColumn);
        if (!item) {
            // If the item does not exist, create it
            item = new QTableWidgetItem();
            table->setItem(row, passwordColumn, item);
        }
        std::string temp = generateRandomPassword(40);
        QString pass =  QString::fromStdString(temp);
        item->setText(pass);
        setStringToSpaces(temp);
        setQStringToSpaces(pass);
    }
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
