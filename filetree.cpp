#include "filetree.h"

#include "time.h"
#ifdef _WIN32
#include "sys/utime.h"
#else
#include "utime.h"
#endif

int modifyLastModifyTime(QDateTime LastModifyTime, QString FilePathName)

{

    if(FilePathName.isEmpty() || !LastModifyTime.isValid() || LastModifyTime.isNull())
        return -1;

    int aYear = LastModifyTime.date().year()-1900;
    int aMonth = LastModifyTime.date().month()-1;
    int aDay = LastModifyTime.date().day();
    int aHour = LastModifyTime.time().hour();
    int aMinute = LastModifyTime.time().minute();
    int aSec = LastModifyTime.time().second();

    struct tm tma = {0};

    tma.tm_year = aYear;
    tma.tm_mon = aMonth;
    tma.tm_mday = aDay;
    tma.tm_hour = aHour;
    tma.tm_min = aMinute;
    tma.tm_sec = aSec;
    tma.tm_isdst = 0;

#ifdef _WIN32
    struct _utimbuf ut;
#else
    struct utimbuf ut;
#endif

    ut.modtime = mktime(&tma);
    ut.actime=mktime(&tma);

    QByteArray ary=FilePathName.toLocal8Bit();

    const char *fileName = ary.data();
    int result=-1;

#ifdef _WIN32
    result=_utime(fileName, &ut );
#else
    result=utime(fileName, &ut );
#endif

    return result;

}

QDataStream& operator<<(QDataStream& os, const TreeFile& info)
{
    os<<info.name<<info.data<<info.m;
    return os;
}

QDataStream& operator>>(QDataStream& is, TreeFile& info)
{
    is>>info.name>>info.data>>info.m;
    return is;
}

FileTree::FileTree()
{

}

void FileTree::Initial(FolderInfo *info)
{
    if(initialized_)
        return;

    root_path_ = info->root_path_;
    rela_path_ = info->relative_path_;

    qDebug()<<"Initial, root_path = "<<root_path_<<", rela_path = "<<rela_path_;

    info_ = info;

    //the depth = info->depth
    depth_ = info->depth_;

    //read files
    for(int i = 0; i < info->files_.size(); i++)
    {
        TreeFile tf;
        tf.name = info->files_[i].file_name;
        files_.push_back(tf);
        //read the whole file
        QFile file(info->absolute_path_ + "/" + info->files_[i].file_name);
        if (!file.open(QIODevice::ReadOnly))
        {
            files_.pop_back();
            continue;
        }

        files_[i].data = file.readAll();
    }

    //subfolders
    for(int i = 0; i < info->sub_folders_.size(); i++)
    {
        FileTree* sub = new FileTree();
        sub->Initial(info->sub_folders_[i]);
        subfolders_.push_back(sub);
    }

    initialized_ = true;
}

//void FileTree::Read()
//{

//}

//only after you open a project can you do that
void FileTree::Update()
{
    //1:parse the diff
}

void FileTree::WriteToFile(QDataStream& s)
{
    if(depth_ == 1)
    {
        //write down the info
        s<<*info_;
    }

    //basic info is written in the info_, so need not to write it
    //write files
    s<<files_;

    //write subfolders
    int sz = subfolders_.size();
    s<<sz;
    for(int i = 0; i < sz; i++)
    {
        subfolders_[i]->WriteToFile(s);
    }
}

void FileTree::ReadFromFileInit(QDataStream &s)
{
    qDebug()<<"void FileTree::ReadFromFileInit(QDataStream &s)";
    depth_ = 1;
    ReadFromFile(s, NULL);
}

void FileTree::SetRoot(QString root)
{
    root_path_ = root;
    info_->root_path_ = root;
    info_->absolute_path_ = root + "/" + info_->relative_path_;
    for(int i = 0; i < subfolders_.size(); i++)
        subfolders_[i]->SetRoot(root);
}

void FileTree::ReadFromFile(QDataStream &s, FolderInfo* info)
{
    if(initialized_)
        return;

    if(depth_ == 1)
    {
        qDebug()<<"read info depth_ = 1";
        //read the fileinfo
        info_ = new FolderInfo();
        s>>*info_;
        qDebug()<<"read info depth_ = 1 finished";
    }
    else
    {
        info_ = info;
    }

    rela_path_ = info_->relative_path_;

    s>>files_;

    qDebug()<<"rela = "<<rela_path_<<", total files = "<<files_.size();

    //read subfolders
    int sz;
    s>>sz;

    qDebug()<<"sub cnt = "<<sz;

    if(sz > 0)
    {
        subfolders_.resize(sz);
        for(int i = 0; i < sz; i++)
        {
            subfolders_[i] = new FileTree();
            subfolders_[i]->depth_ = depth_ + 1;
            subfolders_[i]->ReadFromFile(s, info_->sub_folders_[i]);
        }
    }

    initialized_ = true;
}

void FileTree::Extract()
{
    qDebug()<<"extracting "<<root_path_ + "/" + rela_path_;
    //if the dir exists, just quit it
    if(QFileInfo(root_path_ + "/" + rela_path_).exists())
    {
        return;
    }

    //create the dir; and then extract the files!
    QDir dir(root_path_ + "/" + rela_path_);
    dir.mkpath(root_path_ + "/" + rela_path_);

    for(int i = 0; i < files_.size(); i++)
    {
        QFile f(root_path_ + "/" + rela_path_ + "/" + files_[i].name);

        if(f.open(QIODevice::WriteOnly))
        {
            f.write(files_[i].data);
            f.close();
        }
    }

    //subfolders
    for(int i = 0; i < subfolders_.size(); i++)
    {
        subfolders_[i]->Extract();
    }
}
