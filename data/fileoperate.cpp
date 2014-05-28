/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: fileoperate.cpp
** Description: FileOperate.
**
** Created By: tang, tang@xdt-inc.com
** Created Date: 2013-10-12T20:42:35
**
***************************************************************************************************/

#include "fileoperate.h"

using namespace ALP;

FileOperate::FileOperate()
{
}

/**
 * @brief Get file type by surfix
 * @param surfix
 * @return
 */
int FileOperate::fileType(const QString &fileName)
{

}

FileIO *FileOperate::fileIO() const
{
    return m_fileIO;
}

void FileOperate::setFileIO(FileIO *fileIO)
{
    m_fileIO = fileIO;
}
