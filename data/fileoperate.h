/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: fileoperate.h
** Description: FileOperate.
**
** Created By: tang, tang@xdt-inc.com
** Created Date: 2013-10-12T20:42:35
**
***************************************************************************************************/

#ifndef FILEOPERATE_H
#define FILEOPERATE_H
#include "fileshare.h"
#include "fileio.h"

namespace ALP{

class FileOperate
{
public:
    FileOperate();

protected:
    FileIO *m_fileIO;
    QString m_fileName;
    int m_fileType;
    bool m_isOpen;

    FileIO *fileIO() const;
    void setFileIO(FileIO *fileIO);

    int fileType(const QString &fileName);
};
}

#endif // FILEOPERATE_H
