#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString path;

    QString log, tmps;

    std::string s;

    for(int i = 0; i < argc; i++)
    {
        path = path.fromLocal8Bit(argv[i]);
        //qDebug()<<i<<argc<<path;
        tmps.setNum(i);
        log += tmps + " " + path + "\n";
        if(path.right(5) == ".pdlx")
        {
            w.ReadFromFile(path);
            //QMessageBox::information(&w, path, path);
            //break;
        }
        char c = ' ';
        s += argv[i];
        s += " ";
    }


    return a.exec();
}
