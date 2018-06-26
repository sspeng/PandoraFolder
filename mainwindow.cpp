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
    connect(ui->passwd_le, SIGNAL(textChanged(QString)), this, SLOT(PasswdChanged()));
    connect(&pandora_, SIGNAL(WrongPasswd()), this, SLOT(WrongPasswd()));

    logger_ = GetLoggerPtr();
    logger_->ConnectPTE(ui->log_pte);

    ui->log_pte->setWordWrapMode(QTextOption::NoWrap);
    ui->log_pte->setReadOnly(true);

    this->setWindowTitle("PandoreFolder");

    passwd_dialog_ = new QDialog(this);
    passwd_dialog_->setWindowTitle("Enter the password");
    passwd_dialog_->resize(300, 200);

    passwd_le_ = new QLineEdit(passwd_dialog_);
    passwd_le_->setEchoMode(QLineEdit::Password);
    passwd_le_->setGeometry(50, 50, 200, 20);

    finish_passwd_btn_ = new QPushButton(passwd_dialog_);
    finish_passwd_btn_->setGeometry(120, 140, 60, 30);
    finish_passwd_btn_->setText("OK");
    connect(finish_passwd_btn_, SIGNAL(clicked(bool)), passwd_dialog_, SLOT(close()));
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

    if(pandora_.built_)
        ui->build_project_btn->setText("update");
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

void MainWindow::EnterPasswd()
{
    passwd_dialog_->exec();
    ui->passwd_le->setText(passwd_le_->text());
}

void MainWindow::ReadFromFile(QString path)
{
    //check the fileinfo
    QFileInfo info(path);
    if(!info.isFile())
        return;

    QDir::setCurrent(info.absolutePath());

    EnterPasswd();

    pandora_.pdlx_path_ = path;
    pandora_.ReadFromFile();

    if(pandora_.built_)
        QMessageBox::information(this, "Success", "Successfully decrypted the pdlx file.");

    if(pandora_.built_)
        ui->build_project_btn->setText("update");
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

    if(pandora_.built_)
        QMessageBox::information(this, "Success", "Successfully decrypted the pdlx file.");

    if(pandora_.built_)
        ui->build_project_btn->setText("update");
}

void MainWindow::PasswdChanged()
{
    pandora_.passwd_ = ui->passwd_le->text();
//    qDebug()<<pandora_.passwd_;
}

void MainWindow::WrongPasswd()
{
    QMessageBox::warning(this, "Wrong password", "The password you have entered can not decrypt the pdlx file.");
}
