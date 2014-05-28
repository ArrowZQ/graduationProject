/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: fileio.cpp
** Description: FileIO.
**
** Created By: wangshuo, wangshuo@xdt-inc.com
** Created Date: 2013-10-12T20:42:35
**
***************************************************************************************************/

#include <QDebug>
#include "fileio.h"

using namespace ALP;

FileIO::FileIO()
{
//    m_fileAbsoluteName = fileName;

//    // Open file
//    std::string file = std::string((const char *)fileName.toLocal8Bit());
//    m_fileStream.open(file.c_str(), std::ios_base::binary);

//    // Set file open status
//    setOpen(m_fileStream.is_open());
}

FileIO::~FileIO()
{
    m_fileStream.close();
}

void FileIO::open(const QString &fileName)
{
    m_fileAbsoluteName = fileName;

    // Open file
    std::string file = std::string((const char *)fileName.toLocal8Bit());

    m_fileStream.open(file.c_str(), std::ios_base::binary);

    m_fileWriteStream.open(file.c_str(), std::ios_base::binary | std::ios_base::ate| std::ios_base::in);

    // Set file open status
    setOpen(m_fileStream.is_open());
}

/**
 * @brief Load minimum requied data to memory
 */
bool FileIO::initialization()
{

}

/**
 * @brief Get object list
 * @param type Object type
 * @return
 */
void *FileIO::list(int type)
{

}

/**
 * @brief Get object type
 * @param seq Sequence number of the object in data files
 * @return ObjectType
 */
int FileIO::type(int seq)
{

}

void FileIO::openFile(const QString &fileName)
{

}

/**
 * @brief Get object meta by seq
 * @param seq Sequence number of the object in data files
 * @return
 */
MetaData *FileIO::meta(int seq)
{

}

/**
 * @brief Get object data be seq
 * @param seq Sequence number of the object in data files
 * @return
 */
void *FileIO::data(int seq)
{

}

bool FileIO::updateName(int seq, QString name)
{

}

bool FileIO::updateAliasName(int seq, QString aName)
{

}

bool FileIO::deleteObject(int seq)
{

}

bool FileIO::dicardObject(int seq)
{

}

bool FileIO::recoverObject(int seq)
{

}

bool FileIO::writeChannelData(ALP_OBJECT_ENTRY *entry, ALP_CHANNEL *channel, Data<float, float, float> *data)
{

}

bool FileIO::writeTableData(ALP_OBJECT_ENTRY *entry, QList<ALP_TABLE_FIELD *> listTableField, QList<QPair<int, QList<void *> > > recoders)
{

}

bool FileIO::writeStreamData(ALP_OBJECT_ENTRY *entry, const char *buf, int len)
{

}


/**
 * @brief Get object depths related to object data
 * @param start
 * @param interval
 * @param number
 * @return
 */
QVector<float> *FileIO::depths(float start, float interval, int number)
{
    QVector<float> *vector = new QVector<float>();

    for (int i = 0; i < number; i++) {
        vector->append(start + i * interval);
    }

    return vector;
}

/**
 * @brief Get read status.
 * @return True if file is loaded.
 */
bool FileIO::isRead() const
{
    return m_isRead;
}

/**
 * @brief Set read status
 * @param isRead
 */
void FileIO::setRead(bool isRead)
{
    m_isRead = isRead;
}

/**
 * @brief Get file's absolute name
 * @return
 */
QString FileIO::fileAbsoluteName() const
{
    return m_fileAbsoluteName;
}

/**
 * @brief Set file's absolute name
 * @param fileAbsoluteName
 */
void FileIO::setFileAbsoluteName(const QString &fileAbsoluteName)
{
    m_fileAbsoluteName = fileAbsoluteName;
}

/**
 * @brief Get init status.
 * @return True if file is initialized.
 */
bool FileIO::isInit() const
{
    return m_isInit;
}

/**
 * @brief Set init status
 * @param init
 */
void FileIO::setInit(bool init)
{
    m_isInit = init;
}

/**
 * @brief Get open status
 * @return True if the file is opened
 */
bool FileIO::isOpen() const
{
    return m_isOpen;
}

/**
 * @brief Set open status
 * @param isOpen File open status
 */
void FileIO::setOpen(bool isOpen)
{
    m_isOpen = isOpen;
}

/**
 * @brief Get object type
 * @param attribute
 * @param subattribute
 * @return
 * @todo axp and other files?
 */
int FileIO::objectType(int attribute, int subattribute)
{
    if (attribute == AlpData::ChannelAttribure) {
        if (subattribute >= AlpData::CurveObject && subattribute <= AlpData::NoneObject) {
            return subattribute;
        }
    }
    else if (attribute == AlpData::TableAttribure) {
        return AlpData::TableObject;
    }
    else if (attribute == AlpData::StreamAttribure) {
        return AlpData::StreamObject;
    }

    return AlpData::NoneObject;
}
