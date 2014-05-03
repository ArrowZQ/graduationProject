/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Data Library
** File: fileshare.h
** Description: FileShare.
**
** Created By: micle, micle@xdt-inc.com
** Created Date: 2014-2-14T16:14:00
**
***************************************************************************************************/

#ifndef ALP_FILESHARE_H
#define ALP_FILESHARE_H

#include <QMap>
#include <QString>
#include "data_global.h"
#include "fileio.h"

namespace ALP {

/**
 * @brief The FileShare class. 文件共享类。
 *
 * All opened files should be managed and stored in a pool for sharing.
 *
 * @author Micle Bu
 * @date 2014-02-14
 */
class DATASHARED_EXPORT FileShare
{
public:
    FileShare();
    ~FileShare();

    // Static methods
    static FileIO *file(const QString &fileName);
    static bool addFile(const QString &fileName, FileIO *fileIO);
    static bool removeFile(const QString &fileName);

    static QList<QString> fileList();
    static int fileID(const QString &fileName);

    static QString current();
    static void setCurrent(const QString &fileName);

private:

    // Static properties
    static QList<QPair<QString, FileIO *> > m_fileList;
    static QList<QString> m_nameList;
    static QString m_current;
};

}

#endif // ALP_FILESHARE_H
