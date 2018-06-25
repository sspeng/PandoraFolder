#ifndef LOGGER_H
#define LOGGER_H

#include <QPlainTextEdit>
#include <QApplication>
#include <QDebug>

class Logger
{
public:
    void ConnectPTE(QPlainTextEdit* pte);

    void PushLog(QString s, int indent = 0);
    QString TotalLog();

    friend Logger* GetLoggerPtr();

private:
    QPlainTextEdit* log_pte_ = NULL;
    Logger();
};

Logger* GetLoggerPtr();

#endif // LOGGER_H
