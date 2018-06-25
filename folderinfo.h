#ifndef FOLDERINFO_H
#define FOLDERINFO_H

#include <QMap>
#include <QVector>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QFileInfoList>
#include <algorithm>

#include <QDebug>

#include <QDataStream>

#include <QFile>

#include <QVariant>
#include <QVariantMap>

#include "logger.h"

struct SimpleFileInfo
{
    QString file_name;
    QDateTime modify_time;
    qint64 size;

    QVariantMap m;//for later version if possible

    friend QDataStream& operator<<(QDataStream& os, const SimpleFileInfo& info);
    friend QDataStream& operator>>(QDataStream& is, SimpleFileInfo& info);
};

void ConvertFileInfo(SimpleFileInfo* dst, const QFileInfo* src);

//for std::sort
bool CmpSimpleFileInfo(const SimpleFileInfo& f1, const SimpleFileInfo& f2);
bool CmpFileInfo(const QFileInfo& f1, const QFileInfo& f2);

class FolderInfo
{
public:
    FolderInfo();
    FolderInfo(QString f_name, QString r_path, QString root_path, int depth_);

    void Parse();

    QString folder_name_;//may not be changed
    QString relative_path_;//this will be empty if being root
    QString root_path_;//need not to be stored

    QString absolute_path_;//need not to be stored

    QVector<FolderInfo*> sub_folders_;
    QVector<SimpleFileInfo> files_;

    void WriteToFile(QDataStream& s) const;
    void ReadFromFile(QDataStream& s);
    void SetRoot(QString s);

    int depth_;

    friend QDataStream& operator<<(QDataStream& os, const FolderInfo& info);
    friend QDataStream& operator>>(QDataStream& is, FolderInfo& info);
};

#endif // FOLDERINFO_H
