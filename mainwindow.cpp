#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    ui->setupUi(this);

    ui->folder_name_le->setReadOnly(true);
    connect(ui->select_dir_btn, SIGNAL(clicked(bool)), this, SLOT(SelectPath()));
    connect(ui->build_project_btn, SIGNAL(clicked(bool)), this, SLOT(BuildProject()));
    connect(ui->clear_project_btn, SIGNAL(clicked(bool)), this, SLOT(ClearProject()));
    connect(ui->extract_project_btn, SIGNAL(clicked(bool)), this, SLOT(ExtractProject()));
    connect(ui->read_pdlx_btn, SIGNAL(clicked(bool)), this, SLOT(ReadProject()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SelectPath()
{
    if(pandora_.built_)
        return;

    QString dir_path = QFileDialog::getExistingDirectory();
    qDebug()<<dir_path;

    QFileInfo info(dir_path);
    if(info.isDir())
    {
        //accept this!
        ui->folder_name_le->setText(dir_path);
    }
}

void MainWindow::BuildProject()
{
    qDebug()<<"void MainWindow::BuildProject()";
    if(!pandora_.built_)
    {
        QString folder_path = ui->folder_name_le->text();
        QFileInfo info(folder_path);
        if(info.isDir())
        {
            //build the project
            pandora_.BuildProject(folder_path);
        }
    }
    else
        pandora_.UpdateProject();
}

void MainWindow::ClearProject()
{
    if(pandora_.built_)
        pandora_.ClearProject();
}

void MainWindow::ExtractProject()
{
    if(pandora_.built_)
        pandora_.ExtractProject();
}

void MainWindow::ReadFromFile(QString path)
{
    //check the fileinfo
    QFileInfo info(path);
    if(!info.isFile())
        return;

    QDir::setCurrent(info.absolutePath());

    pandora_.pdlx_path_ = path;
    pandora_.ReadFromFile();
}

void MainWindow::ReadProject()
{
    if(pandora_.built_)
        return;

    QString path=QFileDialog::getOpenFileName(this,"select pdlx file",".","pandora folder project file(*.pdlx)");
    qDebug()<<path;

    QFileInfo info(path);
    if(info.isFile())
    {
        //accept this!
        ui->folder_name_le->setText(path);

        pandora_.pdlx_path_ = path;
        pandora_.ReadFromFile();
    }
}
