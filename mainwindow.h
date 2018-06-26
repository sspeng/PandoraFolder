#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include "folderdiff.h"
#include "pandorafolder.h"
#include <QMessageBox>

#include "logger.h"
#include <QLineEdit>

#include <QDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void ReadFromFile(QString path);

private:
    Ui::MainWindow *ui;

    PandoraFolder pandora_;

    Logger* logger_;

    void EnterPasswd();
    QDialog* passwd_dialog_;
    QLineEdit* passwd_le_;
    QPushButton* finish_passwd_btn_;

private slots:
    void SelectPath();

    void BuildProject();//"build" will be run only once! and then this will act as "update"

    void ClearProject();//clear the directory on the disk

    void ExtractProject();

    void ReadProject();

    void PasswdChanged();

    void WrongPasswd();

};

#endif // MAINWINDOW_H
