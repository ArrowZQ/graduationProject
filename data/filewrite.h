#ifndef FILEWRITE_H
#define FILEWRITE_H

#include "fileshare.h"
#include "alpio.h"
#include <QDebug>
#include <QFile>

using namespace ALP;
class FileWrite
{
public:
    FileWrite() {}

    bool open(const QString &fileName);

    bool writeChannelData(void);
    bool writeTableData(void);
    bool writeStreamData(void);
    bool updateName(int seq, QString name);
    bool updateAliasName(int seq, QString aName);
    bool deleteObject(int seq);
    bool dicardObject(int seq);
    bool recoverObject(int seq);
    bool updateObjectData(int seq,void*);

    FileIO *fileIO() const;
    void setFileIO(FileIO *fileIO);

    int fileType(const QString &fileName);
private:
    FileIO *m_fileIO;
    QString m_fileName;
    int m_fileType;
    bool m_isOpen;
};

#endif // FILEWRITE_H
