#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void loadFromJsonForGUI();
    void exportPasswords();
    void importPasswords();
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void on_generateButton_clicked();

    void on_copyButton_clicked();

    void on_viDataButton_clicked();
    void on_manualSaveButton_clicked();
    void on_masterPassButton_clicked();
    void on_exportButton_clicked();
    void on_importButton_clicked();
};
#endif // MAINWINDOW_H
