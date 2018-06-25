#include "folderdiff.h"

FolderDiff::FolderDiff()
{

}

FolderDiff::FolderDiff(FolderInfo * f1, FolderInfo * f2)
{
    info1_ = f1;
    info2_ = f2;
}

bool CmpTimeSize(const QDateTime& t1, const QDateTime& t2, const qint64& sz1, const qint64& sz2)
{
    if(sz1 == sz2)
    {
        if(t1.secsTo(t2) < 40)
            return true;
        else
            return false;
    }
    else
        return false;
}

void FolderDiff::Parse()
{
    //since the name is sorted, guaranteed from "std::sort(&(v[0]), &(v[0]) +　v.size(), CmpFileInfo);"

    int iter1 = 0, iter2 = 0;

    ////parse files
    while(iter1 < info1_->files_.size() && iter2 < info2_->files_.size())
    {
        if(info1_->files_[iter1].file_name == info2_->files_[iter2].file_name)
        {
            //common condition, the same file name

            if(!CmpTimeSize(info1_->files_[iter1].modify_time, info2_->files_[iter2].modify_time, info1_->files_[iter1].size, info2_->files_[iter2].size))
            {
                //modified
                diff_file_.push_back(info1_->files_[iter1].file_name);

                if(info1_->files_[iter1].modify_time > info2_->files_[iter2].modify_time)
                    file_diff_num_.push_back(ModifiedOld);
                else if(info2_->files_[iter1].modify_time > info2_->files_[iter2].modify_time)
                    file_diff_num_.push_back(ModifiedNew);
                else
                    file_diff_num_.push_back(ModifiedNew);//to be determined

                new_sz_ += info2_->files_[iter2].size;
                new_count_++;
                del_sz_ += info1_->files_[iter1].size;
                del_count_++;

                GetLoggerPtr()->PushLog("diff file:" + info1_->absolute_path_ + "/" + info1_->files_[iter1].file_name + " and " + info2_->absolute_path_ + "/" + info2_->files_[iter2].file_name,
                                        info1_->depth_);
            }
            else
            {
                //not modified

                //cmmcmm->CS("same file:" + info1_->absolute_path_ + "/" + info1_->files_[iter1].file_name + " and " + info2_->absolute_path_ + "/" + info2_->files_[iter2].file_name);
            }

            iter1++;
            iter2++;
        }
        else
        {
            //not the same name.
            if(info1_->files_[iter1].file_name < info2_->files_[iter2].file_name)
            {
                //old folder, a deleted file

                diff_file_.push_back(info1_->files_[iter1].file_name);
                file_diff_num_.push_back(Deleted);

                del_sz_ += info1_->files_[iter1].size;
                del_count_++;

                GetLoggerPtr()->PushLog("deleted file " + info1_->absolute_path_ + "/" + info1_->files_[iter1].file_name,
                                        info1_->depth_);

                iter1++;
            }
            else
            {
                //new folder, a new file

                diff_file_.push_back(info2_->files_[iter2].file_name);
                file_diff_num_.push_back(New);

                new_sz_ += info2_->files_[iter2].size;
                new_count_++;

                GetLoggerPtr()->PushLog("new file " + info2_->absolute_path_ + "/" + info2_->files_[iter2].file_name,
                                        info1_->depth_);

                iter2++;
            }
        }
    }

    //parse the uncompared.
    if(iter1 == info1_->files_.size())//new files left
    {
        //new folder, new files

        for(; iter2 < info2_->files_.size(); iter2++)
        {
            diff_file_.push_back(info2_->files_[iter2].file_name);
            file_diff_num_.push_back(New);

            new_sz_ += info2_->files_[iter2].size;
            new_count_++;

            GetLoggerPtr()->PushLog("new file" + info2_->absolute_path_ + "/" + info2_->files_[iter2].file_name,
                                    info1_->depth_);
        }
    }
    else
    {
        //old folder, old files

        for(; iter1 < info1_->files_.size(); iter1++)
        {
            diff_file_.push_back(info1_->files_[iter1].file_name);
            file_diff_num_.push_back(Deleted);

            del_sz_ += info1_->files_[iter1].size;
            del_count_++;

            GetLoggerPtr()->PushLog("deleted file " + info1_->absolute_path_ + "/" + info1_->files_[iter1].file_name,
                                    info1_->depth_);
        }
    }


    ////parse folders

    iter1 = 0;
    iter2 = 0;

    while(iter1 < info1_->sub_folders_.size() && iter2 < info2_->sub_folders_.size())
    {
        if(info1_->sub_folders_[iter1]->folder_name_ == info2_->sub_folders_[iter2]->folder_name_)
        {
            //the same folder name, enter the folder!

            FolderDiff* fd = new FolderDiff(info1_->sub_folders_[iter1], info2_->sub_folders_[iter2]);
            fd->Parse();

            //but if the folder is the same, there is no need for "push_back"

            if(fd->del_count_ + fd->new_count_ + fd->del_folder_count_ + fd->new_folder_count_ != 0)
            {
                sub_diff_.push_back(fd);

                del_sz_ += fd->del_sz_;
                del_count_ += fd->del_count_;
                del_folder_count_ += fd->del_folder_count_;

                new_sz_ += fd->new_sz_;
                new_count_ += fd->new_count_;
                new_folder_count_ += fd->new_folder_count_;

                GetLoggerPtr()->PushLog("diff dir " + info1_->sub_folders_[iter1]->absolute_path_ + "  " + info2_->sub_folders_[iter2]->absolute_path_,
                                        info1_->depth_);
            }
            else
            {
                delete fd;
                //cmmcmm->CS("same dir " + info1_->sub_folders_[iter1]->absolute_path_ + info2_->sub_folders_[iter2]->absolute_path_);
            }

            iter1++;
            iter2++;
        }
        else
        {
            //not the same name

            if(info1_->sub_folders_[iter1]->folder_name_ < info2_->sub_folders_[iter2]->folder_name_)
            {
                //an old folder is deleted

                diff_dir_.push_back(info1_->sub_folders_[iter1]->folder_name_);
                dir_diff_num_.push_back(Deleted);

                del_folder_count_++;

                GetLoggerPtr()->PushLog("deleted dir1 " + info1_->sub_folders_[iter1]->absolute_path_,
                                        info1_->depth_);

                iter1++;
            }
            else
            {
                //a new folder is created

                diff_dir_.push_back(info2_->sub_folders_[iter2]->folder_name_);
                dir_diff_num_.push_back(New);

                new_folder_count_++;

                GetLoggerPtr()->PushLog("new dir1 " + info2_->sub_folders_[iter2]->absolute_path_,
                                        info1_->depth_);

                iter2++;
            }
        }
    }

    //parse the uncompared
    if(iter1 == info1_->sub_folders_.size())//new files left
    {
        //new folder, new files

        for(; iter2 < info2_->sub_folders_.size(); iter2++)
        {
            diff_dir_.push_back(info2_->sub_folders_[iter2]->folder_name_);
            dir_diff_num_.push_back(New);

            new_folder_count_++;

            GetLoggerPtr()->PushLog("new dir2 " + info2_->sub_folders_[iter2]->absolute_path_,
                                    info1_->depth_);
        }
    }
    else
    {
        //old folder, old files

        for(; iter1 < info1_->sub_folders_.size(); iter1++)
        {
            diff_dir_.push_back(info1_->sub_folders_[iter1]->folder_name_);
            dir_diff_num_.push_back(Deleted);

            del_folder_count_++;

            GetLoggerPtr()->PushLog("deleted dir2 " + info1_->sub_folders_[iter1]->absolute_path_,
                                    info1_->depth_);
        }
    }
}
