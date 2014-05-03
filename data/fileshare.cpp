/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: fileshare.cpp
** Description: FileShare.
**
** Created By: micle, micle@xdt-inc.com
** Created Date: 2014-2-14T10:00:00
**
***************************************************************************************************/

#include "fileshare.h"

using namespace ALP;

/// Shared file list
QList<QPair<QString, FileIO *> > FileShare::m_fileList;
QList<QString> FileShare::m_nameList;
QString FileShare::m_current;

FileShare::FileShare()
{
}

FileShare::~FileShare()
{

}

//======================================= Section: Methods ===========================================
/**
 * @brief Get file's FileIO object by name
 * @param fileName File name
 * @return FileIO object
 */
FileIO *FileShare::file(const QString &fileName)
{
    for (int i = 0; i < m_fileList.count(); i++) {
        QPair<QString, FileIO *> item = m_fileList.at(i);

        if (item.first == fileName) {
            return item.second;
        }
    }

    return NULL;
}

/**
 * @brief Add file to shared file list
 *
 * Set the latest file as current file.
 *
 * @param fileName File name
 * @param fileIO FileIO object
 * @param True if added, false if the file alread in the list
 */
bool FileShare::addFile(const QString &fileName, FileIO *fileIO)
{
    for (int i = 0; i < m_fileList.count(); i++) {
        QPair<QString, FileIO *> item = m_fileList.at(i);

        if (item.first == fileName) {
            return false;
        }
    }

    // Add new file
    QPair<QString, FileIO *> item;
    item.first = fileName;
    item.second = fileIO;

    m_fileList.append(item);
    m_nameList.append(fileName);

    // Set current file
    setCurrent(fileName);

    return true;
}

/**
 * @brief Remove file from shared file list
 * @param fileName File name
 * @return True if removed, false if the file is not in the list
 */
bool FileShare::removeFile(const QString &fileName)
{
    for (int i = 0; i < m_fileList.count(); i++) {
        const QPair<QString, FileIO *> item = m_fileList.at(i);

        if (item.first == fileName) {

            // Remove file and name
            return m_fileList.removeOne(item) && m_nameList.removeOne(fileName);
        }
    }

}

/**
 * @brief Get file list
 * @return
 */
QList<QString> FileShare::fileList()
{
    return m_nameList;
}

/**
 * @brief Get file ID in the file list
 * @param fileName
 * @return
 */
int FileShare::fileID(const QString &fileName)
{
    return m_nameList.indexOf(fileName) + 1;
}

/**
 * @brief Get current file
 *
 * Use current file to get processing data
 *
 * @return Current processing file name
 */
QString FileShare::current()
{
    return m_current;
}

/**
 * @brief Set current file
 * @param fileName File name
 */
void FileShare::setCurrent(const QString &fileName)
{
    m_current = fileName;
}


