#include "filetree.h"

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

void FileTree::Update()
{

}

void FileTree::Extract()
{

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
