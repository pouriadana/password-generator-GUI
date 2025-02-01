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

    // prepare sql table
    int connection_status = sqlite3_open(DB_NAME, &db);
    if (connection_status != SQLITE_OK) {
        qDebug() << "Error connecting to database" << sqlite3_errmsg(db);
    }
    else {
        // connection is stablished, create a table if there isn't one
        const char *create_table =
        "CREATE TABLE IF NOT EXISTS passwords ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "password TEXT NOT NULL, "
        "created at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "comment TEXT"
                                   ");";
        int rc = sqlite3_exec(db, create_table, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            qDebug() << "Error creating table:" << errMsg;
            sqlite3_free(errMsg);
            sqlite3_close(db);
        }
        else {
            qDebug() << "Table checked/created successfully";
        }
    }
    // prepare sql statement to prevent injection
    const char *insertSQL = "INSERT INTO passwords (password, created_at, comment) VALUES (?, datetime('now'), ?);";
    int rc = sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "Preparation initial phase failed\n";
    }
}

MainWindow::~MainWindow()
{
    sqlite3_finalize(insertStmt);
    sqlite3_close(db);
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
    // handle comment here
    std::string comment = "";                                  // TEMPORARY

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

    // Call your password generation logic (adjust this as needed)
    std::string password = generatePassword(length, year, color.toStdString());

    // std::cerr << "Password is: " << password << std::endl;                   // DEBUG

    // // Display the generated password in the UI
    ui->passwordLabel->setText(QString::fromStdString(password));

    // insert into database the {password}, and comment
    int bind_stat[2] = {0, 0};       // record the binding status of 1st and 3rd columns
    bind_stat[0] = sqlite3_bind_text(insertStmt, 1, password.c_str(), -1, SQLITE_TRANSIENT);
    bind_stat[1] = sqlite3_bind_text(insertStmt, 2, comment.c_str(), -1, SQLITE_TRANSIENT);
    if (bind_stat[0] == SQLITE_OK && bind_stat[1] == SQLITE_OK) {
        int rc = sqlite3_step(insertStmt);
        if (rc != SQLITE_DONE) {
            qDebug() << "Insertion failed after successfull binding.";
        }
    }
    else {
        qDebug() << "Binding falied";
        if (bind_stat[0] != SQLITE_OK) {
            qDebug() << "Binding password failed\n";
        }
        if (bind_stat[1] != SQLITE_OK) {
            qDebug() << "Binding comment failed\n";
            qDebug() << "Comment value: " << comment.c_str();
        }
    }
    // Reset the statement for the next use
    sqlite3_reset(insertStmt);
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

