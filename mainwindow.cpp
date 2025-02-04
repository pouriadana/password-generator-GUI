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

/* Global variables */
bool first_execution = true;

/* function declaration */
void saveToJson(const std::string &password, const std::string &comment);
void loadFromJsonForDebug();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dataTable->setVisible(false);
}

MainWindow::~MainWindow()
{
    loadFromJsonForDebug();
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

#ifdef QT_DEBUG
    // std::cerr << "Length: " << length;                                       // DEBUG
    // std::cerr << "\nYear: " << year;                                         // DEBUG
    // std::cerr << "\nFavorite color: " << color.toStdString() << std::endl;   // DEBUG
#endif
    // Call password generation logic
    std::string password = generatePassword(length, year, color.toStdString());

#ifdef QT_DEBUG
    // std::cerr << "Password is: " << password << std::endl;                   // DEBUG
#endif
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

void saveToJson(const std::string &password, const std::string &comment) {
    QString jsonFilePath = "passwords.json";
    QFile file(jsonFilePath);

    QJsonArray jsonArray;
    if (file.open(QIODevice::ReadOnly)) {
        // Read existing data
        QByteArray jsonData = file.readAll();
        QJsonDocument existingDoc = QJsonDocument::fromJson(jsonData);
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
        file.write(newDoc.toJson());
        file.close();
#ifdef QT_DEBUG
        qDebug() << "Password saved successfully!";
#endif
    }
}

#ifdef QT_DEBUG
void loadFromJsonForDebug() {
    QString jsonFilePath = "passwords.json";
    QFile file(jsonFilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open JSON file.";
        return;
    }

    QByteArray jsonData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

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
#endif

void MainWindow::on_viDataButton_clicked()
{
    bool isVisible = ui->dataTable->isVisible();

    /* Logic for asking for password
     * if a global variable named "first_execution" is set to positive
     * then ask for password. check that password with the master pass
     * -word.
    */
    if (first_execution && !isVisible) {
        // check for password validity here
        // ...
        // if entered password and master passwords match,
        // first_execution = false;
        // OK to proceed.
        // otherwise, prevent further execution of the function
    }

    if (!isVisible) {
        loadFromJsonForGUI();  // Load data when showing the frame
        this->resize(this->width() + 400, this->height()); // Adjust width as needed
    } else {
        this->resize(this->width() - 400, this->height());
    }

    ui->dataTable->setVisible(!isVisible);
}

void MainWindow::loadFromJsonForGUI() {
    ui->dataTable->clear();  // Clear existing data
    ui->dataTable->setColumnCount(3); // Set columns for password, created_at, comment
    ui->dataTable->setHorizontalHeaderLabels({"Password", "Created At", "Comment"});

    QFile file("passwords.json");
    if (!file.open(QIODevice::ReadOnly)) {
#ifdef QT_DEBUG
        qDebug() << "Failed to open JSON file.";
#endif
        return;
    }

    QByteArray jsonData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isArray()) {
#ifdef QT_DEBUG
        qDebug() << "Invalid JSON format.";
#endif
        return;
    }

    QJsonArray jsonArray = doc.array();
    ui->dataTable->setRowCount(jsonArray.size());

    for (int i = 0; i < jsonArray.size(); ++i) {
        QJsonObject obj = jsonArray[i].toObject();
        ui->dataTable->setItem(i, 0, new QTableWidgetItem(obj["password"].toString()));
        ui->dataTable->setItem(i, 1, new QTableWidgetItem(obj["created_at"].toString()));
        ui->dataTable->setItem(i, 2, new QTableWidgetItem(obj["comment"].toString()));
    }
}

void MainWindow::on_manualSaveButton_clicked()
{
    // retrieve manual password & comment
    QString mpassword = ui->manualPasswordInput->text();
    QString mcomment = ui->manualCommentInput->text();
    if (mpassword.isEmpty() || mcomment.isEmpty()) {
        // show a warning dialogue
        QMessageBox::warning(this, "Input Required", "Both password and comment fields must be filled out.");
#ifdef QT_DEBUG
        qDebug() << "Cannot save; both fields must be set";
#endif
    }
    else {
        saveToJson(mpassword.toStdString(), mcomment.toStdString());
    }
}

void MainWindow::on_pushButton_clicked()
{

}

