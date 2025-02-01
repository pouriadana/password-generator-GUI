#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sqlite3.h>

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
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    sqlite3 *db = nullptr;
    char *errMsg = nullptr;
    sqlite3_stmt *insertSrmt = nullptr;

private slots:
    void on_generateButton_clicked();

    void on_copyButton_clicked();
};
#endif // MAINWINDOW_H
