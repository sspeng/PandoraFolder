#include "folderinfo.h"

QDataStream& operator<<(QDataStream& os, const SimpleFileInfo& info)
{
    os<<info.file_name<<info.modify_time<<info.size<<info.m;
    return os;
}

QDataStream& operator>>(QDataStream& is, SimpleFileInfo& info)
{
    is>>info.file_name>>info.modify_time>>info.size>>info.m;
    return is;
}

void ConvertFileInfo(SimpleFileInfo* dst, const QFileInfo* src)
{
    dst->file_name = src->fileName();
    dst->modify_time = src->lastModified();
    dst->size = src->size();
}

bool CmpFileInfo(const QFileInfo& f1, const QFileInfo& f2)
{
    if(f1.fileName() < f2.fileName())
        return true;
    else if(f1.fileName() > f2.fileName())
        return false;
    return f1.absoluteFilePath() < f2.absoluteFilePath();
}

bool CmpSimpleFileInfo(const SimpleFileInfo& f1, const SimpleFileInfo& f2)
{
    if(f1.file_name < f2.file_name)
        return true;
    else if(f1.file_name > f2.file_name)
        return false;

    return f1.modify_time < f2.modify_time;
}

FolderInfo::FolderInfo()
{

}

FolderInfo::FolderInfo(QString f_name, QString r_path, QString root_path, int depth)
{
    folder_name_ = f_name;
    relative_path_ = r_path;
    root_path_ = root_path;

    absolute_path_ = root_path_;
    if(!r_path.isEmpty())
        absolute_path_ =absolute_path_ + "/" + r_path;
    this->depth_ = depth;
}

void FolderInfo::Parse()
{
    QDir dir(root_path_ + relative_path_);

    if(!dir.exists())
        return;

    QDir::Filters dir_filters = QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs;

    QFileInfoList list = dir.entryInfoList(dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));

    QVector<QFileInfo> v = list.toVector();

    if(v.size() > 0)//vital
        std::sort(&(v[0]), &(v[0]) + v.size(), CmpFileInfo);

    for(int i = 0; i < v.size(); i++)
    {
        QFileInfo file_info = v[i];

        if(file_info.isSymLink())
        {

        }
        else if(file_info.isDir())
        {
            FolderInfo* fi = new FolderInfo(file_info.fileName(), this->relative_path_ + "/" + file_info.fileName(), this->root_path_,
                                            this->depth_ + 1);

            sub_folders_.push_back(fi);
        }
        else
        {
            SimpleFileInfo sfi;

            ConvertFileInfo(&sfi, &file_info);

            files_.push_back(sfi);

            //total_sz_ += file_info.size();
        }
        //qDebug()<<file_info.absolutePath()<<file_info.fileName();
        //GetLoggerPtr()->PushLog(file_info.absolutePath() + file_info.fileName());
    }

    for(int i = 0; i < sub_folders_.size(); i++)
    {
        sub_folders_[i]->Parse();
    }
}

void FolderInfo::WriteToFile(QDataStream &s) const
{
    //write down the basic info
    s<<folder_name_<<relative_path_<<depth_;

    //files
    s<<files_;
//    for(int i = 0; i < files_.size(); i++)
//    {
//        s<<files_[i];
//    }

    int sz = sub_folders_.size();
    s<<sz;

    //subfolders
    for(int i = 0; i < sub_folders_.size(); i++)
    {
        sub_folders_[i]->WriteToFile(s);
    }
}

void FolderInfo::ReadFromFile(QDataStream &s)
{
    //write down the basic info
    s>>folder_name_>>relative_path_>>depth_;

    //files
    s>>files_;
//    for(int i = 0; i < files_.size(); i++)
//    {
//        s>>files_[i];
//    }

    int sz;
    s>>sz;
    sub_folders_.resize(sz);

    //subfolders
    for(int i = 0; i < sub_folders_.size(); i++)
    {
        sub_folders_[i] = new FolderInfo();
        sub_folders_[i]->ReadFromFile(s);
    }
}

QDataStream& operator<<(QDataStream& os, const FolderInfo& info)
{
    info.WriteToFile(os);
    return os;
}

QDataStream& operator>>(QDataStream& is, FolderInfo& info)
{
    info.ReadFromFile(is);
    return is;
}

void FolderInfo::SetRoot(QString s)
{
    root_path_ = s;
    absolute_path_ = root_path_ + "/" + relative_path_;

    for(int i = 0; i < sub_folders_.size(); i++)
    {
        SetRoot(absolute_path_);
    }
}
