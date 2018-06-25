#include "logger.h"

Logger::Logger()
{

}

void Logger::PushLog(QString s, int indent)
{
    if(log_pte_)
    {
        QString appd_txt;
        appd_txt.resize(indent * 2);
        //qDebug()<<"indent = "<<indent<<", text = "<<s;
        appd_txt.fill(QChar(' '));

        log_pte_->appendPlainText(appd_txt + s);//need not \n
        qApp->processEvents();
    }
}

QString Logger::TotalLog()
{
    return log_pte_->toPlainText();
}

void Logger::ConnectPTE(QPlainTextEdit *pte)
{
    if(pte)
    {
        log_pte_ = pte;
    }
}


Logger* __logger_ptr = NULL;
Logger* GetLoggerPtr()
{
    if(__logger_ptr == NULL)
    {
        __logger_ptr = new Logger();
    }

    return __logger_ptr;
}
