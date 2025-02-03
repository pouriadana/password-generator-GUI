#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>
#include <iostream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QCryptographicHash> // For optional hashing
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "PassGenMainLogic.h"
#define INVALID_INT_VAL -9999
#define INVALID_STR_VAL "EmptyValueForColor"
#define DB_NAME "passwords_database.db"

/* function declaration */
std::string xorEncryptDecrypt(const std::string &data, char key);
void saveToJson(const std::string &password, const std::string &comment);
void loadFromJson();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dataFrame->setVisible(false);
}

MainWindow::~MainWindow()
{
    // sqlite3_finalize(insertStmt);
    // sqlite3_close(db);
    loadFromJson();
    delete ui;
}

void MainWindow::on_generateButton_clicked() {
    // Add logic to handle the button click

    // Retrieve input values
    QString length_string = ui->lengthInput->text();
    QString year_string = ui->yearInput->text();
    QString color = ui->colorInput->text();
    QString comment = ui->commentInput->text();

    // handle empty arguments
    int length = INVALID_INT_VAL;
    int year   = INVALID_INT_VAL;
    if (!length_string.isEmpty()) {                             // if a length was set by the user, convert length string to int
        length = length_string.toInt();
    }
    if (!year_string.isEmpty()) {                               // if a year was set by the user, convert year string to int
        year = year_string.toInt();
    }
    if (color.isEmpty()) {                                      // if no color was set by the user, give a specific value to color
        color = INVALID_STR_VAL;
    }

    // std::cerr << "Length: " << length;                                       // DEBUG
    // std::cerr << "\nYear: " << year;                                         // DEBUG
    // std::cerr << "\nFavorite color: " << color.toStdString() << std::endl;   // DEBUG

    // Call password generation logic
    std::string password = generatePassword(length, year, color.toStdString());

    // std::cerr << "Password is: " << password << std::endl;                   // DEBUG

    // // Display the generated password in the UI
    ui->passwordLabel->setText(QString::fromStdString(password));
    // Save to JSON
    saveToJson(password, comment.toStdString());
}

void MainWindow::on_copyButton_clicked()
{
    // get text from password field in GUI
    QString password = ui->passwordLabel->text();

    // Copy the text to the clipboard
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(password);

    // Optionally, show feedback to the user
    QMessageBox::information(this, "Copied", "Password copied to clipboard!");
}

std::string xorEncryptDecrypt(const std::string &data, char key) {
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] ^= key;
    }
    return result;
}

void saveToJson(const std::string &password, const std::string &comment) {
    QString jsonFilePath = "passwords.json";
    QFile file(jsonFilePath);

    QJsonArray jsonArray;
    if (file.open(QIODevice::ReadOnly)) {
        // Read existing data and decrypt
        QByteArray encryptedData = file.readAll();
        QByteArray decryptedData = xorEncryptDecrypt(encryptedData.toStdString(), 'X').c_str();
        QJsonDocument existingDoc = QJsonDocument::fromJson(decryptedData);
        jsonArray = existingDoc.array();
        file.close();
    }

    // Add new entry
    QJsonObject newEntry;
    newEntry["password"] = QString::fromStdString(password);
    newEntry["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    newEntry["comment"] = QString::fromStdString(comment);
    jsonArray.append(newEntry);

    // Write updated JSON
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument newDoc(jsonArray);
        QByteArray encryptedData = xorEncryptDecrypt(newDoc.toJson().toStdString(), 'X').c_str();
        file.write(encryptedData);
        file.close();
        qDebug() << "Password saved and encrypted successfully!";
    }
}

void loadFromJson() {
    QString jsonFilePath = "passwords.json";
    QFile file(jsonFilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open JSON file.";
        return;
    }

    QByteArray encryptedData = file.readAll();
    QByteArray decryptedData = xorEncryptDecrypt(encryptedData.toStdString(), 'X').c_str();
    QJsonDocument doc = QJsonDocument::fromJson(decryptedData);

    if (!doc.isArray()) {
        qDebug() << "Invalid JSON format.";
        return;
    }

    QJsonArray jsonArray = doc.array();
    for (int i = 0; i < jsonArray.size(); ++i) {
        QJsonObject obj = jsonArray[i].toObject();
        qDebug() << "Password:" << obj["password"].toString();
        qDebug() << "Created At:" << obj["created_at"].toString();
        qDebug() << "Comment:" << obj["comment"].toString();
        qDebug() << "----";
    }
}

void MainWindow::on_viDataButton_clicked()
{
    bool isVisible = ui->dataFrame->isVisible();
    ui->dataFrame->setVisible(!isVisible);
}

