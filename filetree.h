#ifndef FILETREE_H
#define FILETREE_H

#include <folderdiff.h>

//only relative path will be used?!
struct TreeFile
{
    //data structure to store a file within the tree
    QString name;
    QByteArray data;
    QVariantMap m;

    friend QDataStream& operator<<(QDataStream& os, const TreeFile& info);
    friend QDataStream& operator>>(QDataStream& is, TreeFile& info);
};

class FileTree
{
    //some variables to be make private in the future
public:
    FileTree();



    //itself is a folder;
    //containing many files

    QString root_path_;//root path will not be stored; it will be setup whenever a project is open
    QString rela_path_;

    void Initial(FolderInfo* info);//including sub-initial
    bool initialized_ = false;

//    void Read();
    void Update();

    FolderInfo* info_;
    QVector<FileTree*> subfolders_;
    QVector<TreeFile> files_;

    void WriteToFile(QDataStream& s);
    void ReadFromFileInit(QDataStream& s);
    void Extract();
    void SetRoot(QString root);

    int depth_;//the root depth = 1

private:

    void ReadFromFile(QDataStream& s, FolderInfo *info);
};

#endif // FILETREE_H
