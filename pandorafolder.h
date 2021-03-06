#ifndef PANDORAFOLDER_H
#define PANDORAFOLDER_H


#include "folderdiff.h"
#include "filetree.h"
#include <QDataStream>
#include "SimpleBinEncryptUtil.h"
#include <QMessageBox>

using namespace std;


class PandoraFolder : public QObject
{
    Q_OBJECT
public:
    PandoraFolder();

    void BuildProject(QString folder_path);
    void UpdateProject();
    void ClearProject();
    void ExtractProject();

    //variables
    bool built_ = false;
    QString passwd_;

    FolderInfo* info_;
    QString pdlx_path_;//changed in WriteToFile;
    FileTree ftree_;

    void WriteToFile(QString fpath, QString folder_name);
    void ReadFromFile();//app startup

    SimpleBinEncryptUtil enc_;

signals:
    void WrongPasswd();
};

extern const QString ddf_oracle;

#endif // PANDORAFOLDER_H

