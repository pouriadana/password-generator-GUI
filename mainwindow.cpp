#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->generateButton, &QPushButton::clicked, this, &MainWindow::on_generateButton_clicked);

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
    int length = -9999;
    int year   = -9999;
    if (!length_string.isEmpty()) {                             // if a length was set by the user, convert length string to int
        length = length_string.toInt();
    }
    if (!year_string.isEmpty()) {                               // if a year was set by the user, convert year string to int
        year = year_string.toInt();
    }
    if (color.isEmpty()) {                                      // if no color was set by the user, give a specific value to color
        color = "EmptyValueForColor";
    }

    // Call your password generation logic (adjust this as needed)
    std::string password = generatePassword(length, year, color.toStdString());

    // // Display the generated password
    ui->passwordLabel->setText(QString::fromStdString(password));
}
