#include <iostream>
#include <QByteArray>
#include <QClipboard>
#include <QCryptographicHash>
#ifdef QT_DEBUG
#include <QDebug>
#endif
#include <QFile>
#include <QGuiApplication>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "PassGenMainLogic.h"

#define INVALID_INT_VAL -9999
#define INVALID_STR_VAL "EmptyValueForColor"
#define MASTER_PASS_FILE "masterpass.hash"

/* Global variables */
bool allow_viewing = false;
bool is_first_execution = true;

/* function declaration */
void saveToJson(const std::string &password, const std::string &comment);
#ifdef QT_DEBUG
void loadFromJsonForDebug();
#endif
QString hashPassword(const QString &password);                                          // Function to hash a password
QString loadStoredMasterPasswordHash();                                                 // Function to load the stored master password hash
void storeMasterPassword(const QString &password);                                      // Function to store the master password securely
QByteArray encryptData(const QByteArray &data, const QString &passwordHash);            // use XOR and master password's hash to encrypt JSON
QByteArray decryptData(const QByteArray &encryptedData, const QString &passwordHash);   // use XOR and master password's hash to decrypt


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dataTable->setVisible(false);
}

MainWindow::~MainWindow()
{
#ifdef QT_DEBUG
    loadFromJsonForDebug();
#endif
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

    /* TODO
     * if password_hash file exists, we will decrypt the JSON
     * if it doesn't, we will just read it as is.
     * if password_hash exists, JSON is guaranteed to be encrypted */
    QByteArray jsonData;
    QJsonArray jsonArray;

    std::filesystem::path masterPassFile(MASTER_PASS_FILE);
    bool isEncrypted = std::filesystem::exists(masterPassFile);
    QString masterHash = loadStoredMasterPasswordHash();
    if (isEncrypted) {
        // decrypt json
        // Read and decrypt the JSON file
        if (file.open(QIODevice::ReadOnly)) {
            jsonData = file.readAll();
            jsonData = decryptData(jsonData, masterHash); // Decrypt using the master password hash
            file.close();

            QJsonDocument existingDoc = QJsonDocument::fromJson(jsonData);
            jsonArray = existingDoc.array();
        }
    }
    else {
        // Read JSON as is (plaintext)
        if (file.open(QIODevice::ReadOnly)) {
            jsonData = file.readAll();
            QJsonDocument existingDoc = QJsonDocument::fromJson(jsonData);
            jsonArray = existingDoc.array();
            file.close();
        }
    }

    // QJsonArray jsonArray;
    // if (file.open(QIODevice::ReadOnly)) {
    //     // Read existing data
    //     QByteArray jsonData = file.readAll();
    //     QJsonDocument existingDoc = QJsonDocument::fromJson(jsonData);
    //     jsonArray = existingDoc.array();
    //     file.close();
    // }

    // Add new entry
    QJsonObject newEntry;
    newEntry["password"] = QString::fromStdString(password);
    newEntry["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    newEntry["comment"] = QString::fromStdString(comment);
    jsonArray.append(newEntry);

    /* TODO
     * Encrypt JSON using masterpass hash */
    // Write updated JSON
    // Convert JSON array to a QJsonDocument
    QJsonDocument newDoc(jsonArray);
    jsonData = newDoc.toJson();

    // Encrypt JSON if a master password is set
    if (isEncrypted) {
        jsonData = encryptData(jsonData, masterHash);  // Encrypt using the stored hash
    }

    // Write updated (encrypted or plaintext) JSON to file
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonData);
        file.close();
#ifdef QT_DEBUG
        qDebug() << "Password saved successfully!";
#endif
    } else {
        QMessageBox::critical(nullptr, "Error", "Failed to write to JSON file.");
    }
}

#ifdef QT_DEBUG
void loadFromJsonForDebug() {
    QString jsonFilePath = "passwords.json";
    QFile file(jsonFilePath);

    /* TODO
     * if password_hash file exists, we will decrypt the JSON
     * if it doesn't, we will just read it as is.
     * if password_hash exists, but JSON is not encrypted, read JSON as is. */

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open JSON file.";
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    /* TODO DONE
     * if password_hash file exists, we will decrypt the JSON
     * if it doesn't, we will just read it as is.
     * if password_hash exists, JSON is guaranteed to be encrypted */
    std::filesystem::path filePath(MASTER_PASS_FILE);
    if (std::filesystem::exists(filePath)) {
        // decrypt the JSON
        // Load stored master password hash
        QString storedHash = loadStoredMasterPasswordHash();
        qDebug() << "Before decryption for debug";
        jsonData = decryptData(jsonData, storedHash);
    }
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

    if (!isVisible && is_first_execution) {
        QString stored_hash = loadStoredMasterPasswordHash();
        QString user_password_input;
        user_password_input = QInputDialog::getText(this, "Enter master password",
                                                    "Enter your master password to view the saved data",
                                                    QLineEdit::Password);
        // check password validity
        if (hashPassword(user_password_input) == stored_hash) {
            is_first_execution = false;
            allow_viewing = true;
        }
        else {
            std::filesystem::path filePath("masterpass.hash");
            if (std::filesystem::exists(filePath)) {
                QMessageBox::warning(this, "Wrong Password", "Master password is wrong");
            }
            else {
                QMessageBox::warning(this, "No master password set", "Master password is not set");
            }
        }
    }
    if (allow_viewing) {
        if (!isVisible) {
            loadFromJsonForGUI();  // Load data when showing the frame
            this->resize(this->width() + 400, this->height()); // Adjust width as needed
        }
        else {
            this->resize(this->width() - 400, this->height());
        }
    }
    if (!(isVisible == false && allow_viewing == false)) // if the user fails to enter a correct password to view the data
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
    file.close();

    /* TODO DONE
     * if password_hash file exists, we will decrypt the JSON
     * if it doesn't, we will just read it as is.
     * if password_hash exists, JSON is guaranteed to be encrypted */
    std::filesystem::path filePath(MASTER_PASS_FILE);
    if (std::filesystem::exists(filePath)) {
        // decrypt the JSON
        // Load stored master password hash
        QString storedHash = loadStoredMasterPasswordHash();
        qDebug() << "Before decryption";
        jsonData = decryptData(jsonData, storedHash);
    }
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

void MainWindow::on_masterPassButton_clicked()
{
    QString storedHash = loadStoredMasterPasswordHash();
    QString currentPass, newPass;

    // If no master password has been set, prompt the user to create one
    if (storedHash.isEmpty()) {
        newPass = QInputDialog::getText(this, "Set Master Password",
                                        "No master password found.\nSet a new master password:",
                                        QLineEdit::Password);
        if (newPass.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Master password cannot be empty.");
            return;
        }
        storeMasterPassword(newPass);
        QMessageBox::information(this, "Success", "Master password set successfully!");
        /* TODO DONE
         * JSON is unencrypted at this point, encrypt the unsecured JSON file
         * Do the encryption here */
        QFile jsonFile("passwords.json");
        if (jsonFile.exists()) {
            if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray jsonData = jsonFile.readAll();
                jsonFile.close();

                // Encrypt JSON data using the hash of the new master password
                QString masterHash = hashPassword(newPass);  // Assuming generateHash() returns a hex-encoded hash
                QByteArray encryptedData = encryptData(jsonData, masterHash);  // Encrypt using hash as key

                // Overwrite the original JSON file with encrypted data
                if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    jsonFile.write(encryptedData);
                    jsonFile.close();
                } else {
                    QMessageBox::critical(this, "Error", "Failed to write encrypted JSON file.");
                }
            } else {
                QMessageBox::critical(this, "Error", "Failed to read JSON file.");
            }
        }
    }
    else { // If a master password exists, require authentication before setting a new one
        currentPass = QInputDialog::getText(this, "Current Master Password Required",
                                            "Enter your current master password:",
                                            QLineEdit::Password);

        // Verify current password
        if (hashPassword(currentPass) != storedHash) {
            QMessageBox::critical(this, "Error", "Incorrect master password!");
            return;
        }

        // If verification succeeds, ask for the new master password
        newPass = QInputDialog::getText(this, "Set New Master Password",
                                        "Enter your new master password:",
                                        QLineEdit::Password);
        if (newPass.isEmpty()) {
            QMessageBox::warning(this, "Warning", "New master password cannot be empty.");
            return;
        }

        /* TODO
         *  Decrypt the JSON using the previous password */
        // read the encrypted JSON file
        QByteArray decryptedData;
        QFile jsonFile("passwords.json");
        if (jsonFile.exists()) {
            if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray jsonData = jsonFile.readAll();
                jsonFile.close();

                // decrypt it
                QString hash = loadStoredMasterPasswordHash();
                decryptedData = decryptData(jsonData, hash);
            }
        }
        storeMasterPassword(newPass);
        QMessageBox::information(this, "Success", "Master password updated successfully!");
        /* TODO
         * Encrypt the JSON using the new password here */
        QString hash = loadStoredMasterPasswordHash();
        QByteArray encryptedData = encryptData(decryptedData, hash);
        // Overwrite the original JSON file with encrypted data
        if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            jsonFile.write(encryptedData);
            jsonFile.close();
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to write encrypted JSON file.");
        }
    }
}

QString hashPassword(const QString &password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// Function to load the stored master password hash
QString loadStoredMasterPasswordHash() {
    QFile file(MASTER_PASS_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        return ""; // No master password set
    }
    QString hash = file.readAll();
    file.close();
    return hash;
}

// Function to store the master password securely
void storeMasterPassword(const QString &password) {
    QFile file(MASTER_PASS_FILE);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "Error", "Failed to save master password!");
        return;
    }
    file.write(hashPassword(password).toUtf8());
    file.close();
}

QByteArray encryptData(const QByteArray &data, const QString &passwordHash) {
    QByteArray key = QCryptographicHash::hash(passwordHash.toUtf8(), QCryptographicHash::Sha256);
    QByteArray encryptedData = data;

    for (int i = 0; i < data.size(); i++) {
        encryptedData[i] = data[i] ^ key[i % key.size()];
    }

    return encryptedData.toBase64();  // Encode for safe storage
}

QByteArray decryptData(const QByteArray &encryptedData, const QString &passwordHash) {
    QByteArray key = QCryptographicHash::hash(passwordHash.toUtf8(), QCryptographicHash::Sha256);
    QByteArray decodedData = QByteArray::fromBase64(encryptedData);

    QByteArray decryptedData = decodedData;
    for (int i = 0; i < decodedData.size(); i++) {
        decryptedData[i] = decodedData[i] ^ key[i % key.size()];
    }

    return decryptedData;
}
