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
    sqlite3 *db = nullptr;                          // the conection to database
    char *errMsg = nullptr;                         // passed to sqlite3_exec()
    sqlite3_stmt *insertStmt = nullptr;             // compiled statement during preparation

private slots:
    void on_generateButton_clicked();

    void on_copyButton_clicked();
    void on_colorInput_cursorPositionChanged(int arg1, int arg2);
};
#endif // MAINWINDOW_H
