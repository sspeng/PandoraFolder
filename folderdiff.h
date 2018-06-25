#ifndef FOLDERDIFF_H
#define FOLDERDIFF_H

#include "folderinfo.h"

class FolderDiff
{
public:
    enum DiffNum
    {
        Deleted = 1,
        ModifiedOld = 2,//file in the old_info_ is newer
        ModifiedNew = 3,//file in the new_info_ is newer
        New = 4
    };


    FolderDiff();
    FolderDiff(FolderInfo*, FolderInfo*);

    FolderInfo* info1_;
    FolderInfo* info2_;
    //from this, we can know two root_path and the relative_path

    QVector<FolderDiff*> sub_diff_;

    QVector<int> file_diff_num_;
    QVector<QString> diff_file_;
    QVector<QString> diff_dir_;
    QVector<int> dir_diff_num_;//only 1 and 3

    void Parse();

    //statistics
    qint64 del_sz_ = 0;
    int del_count_ = 0;
    int del_folder_count_ = 0;

    qint64 new_sz_ = 0;
    int new_count_ = 0;
    int new_folder_count_ = 0;

    bool finished_ = false;
};

#endif // FOLDERDIFF_H
