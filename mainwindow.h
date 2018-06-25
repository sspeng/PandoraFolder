#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include "folderdiff.h"
#include "pandorafolder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    PandoraFolder pandora_;

private slots:
    void SelectPath();

    void BuildProject();//"build" will be run only once! and then this will act as "update"

    void ClearProject();//clear the directory on the disk

    void ExtractProject();
};

#endif // MAINWINDOW_H
