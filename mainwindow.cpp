#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>
#include <sqlite3.h>
#include <iostream>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "PassGenMainLogic.h"
#define INVALID_INT_VAL -9999
#define INVALID_STR_VAL "EmptyValueForColor"
#define DB_NAME "passwords_database.db"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_generateButton_clicked() {
    // Add logic to handle the button click here
    // For example, display a message or process input fields
    // QString generatedPassword = "GeneratedPassword123";

    // // Update the QLabel text
    // ui->passwordLabel->setText(generatedPassword);

    // Retrieve input values
    QString length_string = ui->lengthInput->text();
    QString year_string = ui->yearInput->text();
    QString color = ui->colorInput->text();

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
    std::cerr << "Length: " << length;                                       // DEBUG
    std::cerr << "\nYear: " << year;
    std::cerr << "\nFavorite color: " << color.toStdString() << std::endl;
    // Call your password generation logic (adjust this as needed)
    std::string password = generatePassword(length, year, color.toStdString());
    // std::cerr << "Password is: " << password << std::endl;                   // DEBUG
    // // Display the generated password
    ui->passwordLabel->setText(QString::fromStdString(password));

    sqlite3 *db;
    char *errMsg = nullptr;
    int rc = sqlite3_open(DB_NAME, &db);
    if (rc != SQLITE_OK) {
        qDebug() << "Failed to read database" << sqlite3_errmsg(db);
    }
    else {
        qDebug() << "Database read successfully\n";

        // create sql table if database does not exist
        // Create table if it doesn't exist
        const char *sql = "CREATE TABLE IF NOT EXISTS passwords ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "password TEXT NOT NULL, "
                          "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
                          "comment TEXT"
                          ");";

        rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            qDebug() << "Error creating table:" << errMsg;
            sqlite3_free(errMsg);
        } else {
            qDebug() << "Table checked/created successfully";
        }
    }

    // insert into database, the {password}, current data, and time
    std::string sql_cmd = "INSERT INTO passwords (password, created_at) VALUES ('" + password + "', datetime('now'));";
    const char *sql = sql_cmd.c_str();

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        qDebug() << "Error inserting data:" << errMsg;
        sqlite3_free(errMsg);
    } else {
        qDebug() << "Password inserted successfully!";
    }

    sqlite3_close(db);
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

