#ifndef FILEWRITE_H
#define FILEWRITE_H

#include "fileshare.h"
#include "alpio.h"
#include "fileshare.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>

using namespace ALP;
class FileWrite
{
public:
    FileWrite();

    bool open(const QString &fileName);

    bool writeChannelData(ALP_OBJECT_ENTRY *entry, ALP_CHANNEL *channel, Data<float,float,float> *data);
    bool writeTableData(ALP_OBJECT_ENTRY *entry, QList<ALP_TABLE_FIELD *> listTableField, QList<QPair<int, QList<void *> > > recoders);
    bool writeStreamData(ALP_OBJECT_ENTRY *entry, const char *buf,int len);
    bool updateName(int seq, QString name);
    bool updateAliasName(int seq, QString aName);
    bool deleteObject(int seq);
    bool dicardObject(int seq);
    bool recoverObject(int seq);
    bool updateObjectData(int seq,void*);

    FileIO *fileIO() const;
    void setFileIO(FileIO *fileIO);

    int fileType(const QString &fileName);
    QString createNewFile(QString fileName, QString path, FileIO::FileType type);
private:
    FileIO *m_fileIO;
    QString m_fileName;
    int m_fileType;
    bool m_isOpen;
};

#endif // FILEWRITE_H
