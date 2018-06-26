#include "pandorafolder.h"

inline bool DeleteFileOrFolder( const QString& strPath )
{
    if( strPath.isEmpty() || !QDir().exists( strPath ) )
        return false;

    QFileInfo fileInfo( strPath );

    if( fileInfo.isFile() )
        QFile::remove( strPath );
    else if( fileInfo.isDir() )
    {
        QDir qDir( strPath );
        qDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
        QFileInfoList fileInfoLst = qDir.entryInfoList();
        foreach( QFileInfo qFileInfo, fileInfoLst )
        {
            if( qFileInfo.isFile() )
                qDir.remove( qFileInfo.absoluteFilePath() );
            else
            {
                DeleteFileOrFolder( qFileInfo.absoluteFilePath() );
                qDir.rmdir( qFileInfo.absoluteFilePath() );
            }
        }
        qDir.rmdir( fileInfo.absoluteFilePath() );
    }

    return true;
}

PandoraFolder::PandoraFolder()
{
    //passwd_ = "yooooooo!";
}

void PandoraFolder::BuildProject(QString folder_path)
{
    if(built_)
        return;
    QFileInfo info(folder_path);
    if(info.isDir())
    {
        //build the project
        info_ = new FolderInfo("", "", folder_path, 1);
        info_->Parse();

        qDebug()<<info_->folder_name_;

        //and then write it to file tree
        ftree_.Initial(info_);
        qDebug()<<"initial complete";

        WriteToFile(info.absolutePath() + "/" + info.fileName() + ".pdlx", info.fileName());

        built_ = true;

        qDebug()<<"build finished";
    }
}

//from disk to ram, update the info in the ram
void PandoraFolder::UpdateProject()
{
    qDebug()<<"void PandoraFolder::UpdateProject()";

    QFileInfo check_dir(ftree_.root_path_);
    if(check_dir.isDir())
    {
        GetLoggerPtr()->PushLog("Dir = " + ftree_.root_path_ + "Begin update the project in the RAM");
    }
    else
    {
        GetLoggerPtr()->PushLog("Dir not found!");
        return;
    }

    //parse the folder
    FolderInfo* new_info = new FolderInfo("", "", ftree_.root_path_, 1);
    new_info->Parse();

    //compare the diff
    FolderDiff* diff = new FolderDiff(ftree_.info_, new_info);

    diff->Parse();

    ftree_ = FileTree();
    ftree_.Initial(new_info);

    //update the ftree_ according to the diff
}

void PandoraFolder::ClearProject()
{
    qDebug()<<"void PandoraFolder::ClearProject()"<<built_<<info_->absolute_path_;
    if(built_ && (!info_->absolute_path_.isEmpty()))
        DeleteFileOrFolder(info_->absolute_path_);
}

//from ram to disk, extract the folder
void PandoraFolder::ExtractProject()
{
    ftree_.Extract();
}

//write down the pdlx file
void PandoraFolder::WriteToFile(QString fpath, QString folder_name)
{
    //default name as <dir_name>.pdlx

//    QString folder_name = info_.folder_name_;
    qDebug()<<"void PandoraFolder::WriteToFile(QString fpath)"<<fpath;
    QFile file(fpath);
    if(file.open(QIODevice::WriteOnly))
    {
        QByteArray data_before;

        QDataStream s(&data_before, QIODevice::WriteOnly);

        ftree_.WriteToFile(s);

        QDataStream out(&file);

        QByteArray ddf_before = ddf_oracle.toLocal8Bit();
        //check
        //qDebug()<<QString::fromLocal8Bit(ddf_before.data());

        enc_.Encrypt(ddf_before.data(), ddf_before.data(), ddf_before.size() * sizeof(char), passwd_.toStdString());
        enc_.Encrypt(data_before.data(), data_before.data(), data_before.size() * sizeof(char), passwd_.toStdString());

        out<<ddf_before;

        out<<folder_name;

        out<<data_before;

        file.close();

        pdlx_path_ = fpath;
    }
    else
    {
        qDebug()<<"fail to write file!";
    }
}

//from disk to ram, load the pdlx
void PandoraFolder::ReadFromFile()
{
    //check the passwd
    if(pdlx_path_.isEmpty())
        return;
    QFile f(pdlx_path_);
    if(f.open(QIODevice::ReadOnly))
    {
        QByteArray ddf_after, data_after;
        QString ddf_dec;
        QDataStream in(&f);

        in>>ddf_after;

        enc_.Decrypt(ddf_after.data(), ddf_after.data(), ddf_after.size() * sizeof(char), passwd_.toStdString());

        ddf_dec = QString::fromLocal8Bit(ddf_after);

        qDebug()<<"ddf_dec:";
        //qDebug()<<ddf_dec;

        //check if the passwd is right
        if(ddf_dec == ddf_oracle)
        {
            qDebug()<<"oracle passed";

            QString folder_name;
            in>>folder_name;

            in>>data_after;
            enc_.Decrypt(data_after.data(), data_after.data(), data_after.size() * sizeof(char), passwd_.toStdString());

            QDataStream s(&data_after, QIODevice::ReadOnly);

            ftree_.ReadFromFileInit(s);

            QFileInfo info(pdlx_path_);
            ftree_.SetRoot(info.absolutePath() + "/" + folder_name);

            built_ = true;
            info_ = ftree_.info_;
        }
        else
        {
            //qDebug()<<"Wrong passwd!!!";
            emit WrongPasswd();
        }

        f.close();
    }
}


const QString ddf_oracle = "My name is Van, I'm an artist, I'm a performance artist. I'm hired to people to fulfill their fantasies, their deep dark fantasies. I was gonna be a movie star y'know, modeling and acting. After a hundred and two additions and small parts I decided y'know I had enough, Then I got in to Escort world. The client requests contain a lot of fetishes, so I just decided to go y'know... full Master and change my entire house into a dungeon... Dungeon Master now with a full dungeon in my house and It's going really well. Fisting is 300 bucks and usually the guy is pretty much hard on pop to get really relaxed y'know and I have this long latex glove that goes all the way up to my armpit and then I put on a surgical latex glove up to my wrist and just lube it up and it's a long process y'know to get your whole arm up there but it's an intense feeling for the other person I think for myself too, you go in places that even though it's physical with your hand but for some reason it's also more emotional it's more psychological too and we both get you know to the same place it's really strange at the same time and I find sessions like that really exhausting. I don't know I feel kinda naked because I am looking at myself for the first time, well not myself but this aspect of my life for the first time and it's been harsh... three to five years already? I never thought about it... Kinda sad I feel kinda sad right now, I don't know why.";
