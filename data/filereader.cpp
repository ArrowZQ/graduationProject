/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Data Library
** File: filereader.h
** Description: FileReader.
**
** Created By: micle, micle@xdt-inc.com
** Created Date: 2014-3-21T15:00:00
**
***************************************************************************************************/

#include "filereader.h"

#include <QFile>
#include <QFileInfo>
#include <QHashIterator>

using namespace ALP;

FileReader::FileReader() :
    m_fileIO(NULL)
{
    m_isOpen = false;
}

FileReader::~FileReader()
{
    //FileShare::removeFile(m_fileName);
}

/**
 * @brief Open given file for reading
 *
 * Make sure onle a single file is opened.
 *
 * @param fileName File name
 * @return True if open file successfully
 */
bool FileReader::open(const QString &fileName)
{
    // Clear stored data
    clearData();

    // File checking
    if (!QFile::exists(fileName)) {
        m_isOpen = false;

        return m_isOpen;
    }

    m_fileName = fileName;
    m_fileType = fileType(fileName);
    m_fileIO = FileShare::file(fileName);

    if (!m_fileIO) {
        switch (m_fileType) {
        case ALPFile:
        {
            AlpIO *alp = new AlpIO();
            alp->openFile(fileName);
            m_fileIO = alp;
            m_isOpen = m_fileIO->isOpen();
            break;
        }
        case WISFile:
            break;
        case AXPFile:
            break;
        default:
            return false;
        }

        // Add file to fileshare
        if (!FileShare::addFile(fileName, m_fileIO)) {
            return false;
        }
    }
    else {
        // File is already opened
        m_isOpen = true;
    }

    return m_isOpen;
}

/**
 * @brief Get object list
 * @param type Object type
 * @return QVector<QPair<QString, int> *>
 */
QVector<DataListItem> FileReader::list(int type)
{
    QVector<DataListItem> vector;

    if (!m_isOpen) {
        return vector;
    }

    // Object Type
    if (m_fileType == FileReader::ALPFile) {
        QList<ALP_OBJECT_ENTRY *> *list = static_cast<QList<ALP_OBJECT_ENTRY *> *>(m_fileIO->list(type));
        if (!list) {
            return vector;
        }

        for (int i = 0; i < list->count(); i++) {
            DataListItem item;
            item.Name = list->at(i)->Name;
            item.Type = objectType(list->at(i)->Attribute, list->at(i)->SubAttribute);
            item.Seq = list->at(i)->Position;

            vector.append(item);
        }

        return vector;
    }

    return vector;
}


/**
 * @brief Get meta by seq
 *
 * Use dynamic properties to store meta
 *
 * @param seq Sequence number of the object in data files
 * @return MetaData
 */
MetaData *FileReader::meta(int seq)
{
    MetaData *meta = new MetaData;

    if (m_isOpen) {
        meta = m_fileIO->meta(seq);
    }

    return meta;
}

/**
 * @brief Get metadata by name
 *
 * Name may duplicate.
 *
 * @param seq Sequence number of the object in data files
 * @return MetaData, return the first match.
 */
MetaData *FileReader::meta(const QString &name)
{
    return meta(objectSequence(name));
}

/**
 * @brief Get object data by seq
 *
 * The object type will be automatically processed.
 * User should know object type to use these data.
 *
 * @param name Sequence number of the object in data files
 * @return Data<T1, T2, T3>
 */
void *FileReader::data(int seq)
{
    if (!m_isOpen) {
        return NULL;
    }

    // Object Type
    return m_fileIO->data(seq);
}

/**
 * @brief Get object data by name
 *
 * Name may duplicate.
 *
 * @param seq Sequence number of the object in data files
 * @return Data<T1, T2, T3>, return the first match.
 */
void *FileReader::data(const QString &name)
{
    return data(objectSequence(name));
}

/**
 * @brief Get curve data by seq
 *
 * This should be used to avoid duplicated names.
 *
 * @param seq Sequence number of the object in data files
 * @return QPolygonF, y - depth
 * @todo object type, always WaveObject for array???
 */
QPolygonF FileReader::curveData(int seq)
{
    QPolygonF data;
    int type = m_fileIO->type(seq);

    // Check object type
    if (type != FileIO::CurveObject || !m_isOpen) {
        return data;
    }

    // Object Type
    if (m_fileType == FileReader::ALPFile) {
        Data<float,float,float> *d = static_cast<Data<float,float,float>*>(m_fileIO->data(seq));

        if (!d) {
            return data;
        }

        QMapIterator<float,QList<QPair<float,float>*>*> i(d->data);
        while(i.hasNext()) {
            i.next();
            float x = i.value()->first()->second;
            float y = i.key();

            if (x > -10e+5 && y > -10e+5) {
                QPointF point(x, y);
                data.append(point);
            }
        }
    }

    return data;
}

/**
 * @brief Get curve data by name
 *
 * Name may duplicate.
 *
 * @param name Object name
 * @return QPolygonF, return the first match.
 */
QPolygonF FileReader::curveData(const QString &name)
{
    return curveData(objectSequence(name));
}

/**
 * @brief Get array data by seq and index
 * @param seq Sequence number of the object in data files
 * @param index Array index
 * @return QPolygonF, y represents depth
 */
QPolygonF FileReader::arrayData(int seq, int &count, int index)
{
    QPolygonF data;
    int type = m_fileIO->type(seq);

    // Check object type
    if (type != FileIO::WaveObject || !m_isOpen) {
        return data;
    }

    // Read data from file if data has no been stored
    Data<float,float,float> *d = multiDimensionData(seq);

    if (!d) {
        // Object Type
        if (m_fileType == FileReader::WISFile) {
            d = static_cast<Data<float,float,float>*>(m_fileIO->data(seq));

            if (!d) {
                return data;
            }
        }

        // Store data for reuse.
        m_multiDimensionData.insert(seq, d);
    }

    // Prepare QPolygonF data
    QMapIterator<float, QList<QPair<float,float> *> *> iterator(d->data);
    while(iterator.hasNext()) {
        iterator.next();

        QList<QPair<float,float> *> *timeValues = iterator.value();
        if (index >= timeValues->count()) {
            break;
        }

        // Get data
        float x = timeValues->at(index)->second;
        float y = iterator.key();
        count = timeValues->count();

        // Data preprocessing
        if (x > -10e+5 && y > -10e+5) {
            QPointF point(x, y);
            data.append(point);
        }
    }

    return data;
}

/**
 * @brief Get array data by name
 *
 * Name may duplicate.
 *
 * @param name Object name
 * @param index Array index
 * @return QPolygonF, return the first match.
 */
QPolygonF FileReader::arrayData(const QString &name, int &count, int index)
{
    return arrayData(objectSequence(name), count, index);
}

/**
 * @brief Get wave data by seq
 * @param seq Sequence number of the object in data files
 * @todo old
 * @return QList<QPair<float, QList<float> > >
 */
QList<QPair<float, QList<float> > > FileReader::waveData(int seq)
{
    QList<QPair<float, QList<float> > > data;
    int type = m_fileIO->type(seq);

    // Check object type
    if (type != FileIO::WaveObject || !m_isOpen) {
        return data;
    }

    // Object Type
    if (m_fileType == FileReader::WISFile) {
        Data<float,float,float> *d = static_cast<Data<float,float,float>*>(m_fileIO->data(seq));

        if (!d) {
            return data;
        }
    }

    return data;
}

/**
 * @brief Get wave data by name
 *
 * Name may duplicate.
 *
 * @param seq Sequence number of the object in data files
 * @return QList<QPair<float, QList<float> > >, return the first match.
 */
QList<QPair<float, QList<float> > > FileReader::waveData(const QString &name)
{
    return waveData(objectSequence(name));
}

//======================================= Section: Private Methods =============================================
/**
 * @brief Get file type by surfix
 * @param surfix
 * @return
 */
int FileReader::fileType(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QString surfix = "." + fileInfo.suffix().toLower();

    if (surfix == ".wis") {
        return FileReader::WISFile;
    }
    else if (surfix == ".axp") {
        return FileReader::AXPFile;
    }
    else if (surfix == ".alp") {
        return FileReader::ALPFile;
    }
    else {
        return FileReader::NoneFile;
    }
}

/**
 * @brief Get object sequence number
 *
 * Name may duplicate.
 *
 * @param name Object name
 * @return Sequence number of the object in data files
 */
int FileReader::objectSequence(const QString &name)
{
    int seq = -1;
    if (!m_isOpen) {
        return seq;
    }

    QList<ALP_OBJECT_ENTRY *> *allList = static_cast<QList<ALP_OBJECT_ENTRY *> *>(m_fileIO->list());

    if (!allList) {
        return seq;
    }

    for (int i = 0; i < allList->count(); i++) {
        if (allList->at(i)->Name == name) {
            seq = allList->at(i)->Position;
        }
    }

    return seq;
}

/**
 * @brief Get object type
 * @param attribute
 * @param subattribute
 * @return
 * @todo axp and other files?
 */
int FileReader::objectType(int attribute, int subattribute)
{
    if (attribute == AlpIO::ChannelAttribure) {
        if (subattribute >= AlpIO::CurveObject && subattribute <= AlpIO::NoneObject) {
            return subattribute;
        }
    }
    else if (attribute == AlpIO::TableAttribure) {
        return AlpIO::TableObject;
    }
    else if (attribute == AlpIO::StreamAttribure) {
        return AlpIO::StreamObject;
    }

    return AlpIO::NoneObject;
}

/**
 * @brief FileReader::clearData
 */
void FileReader::clearData()
{
    m_multiDimensionData.clear();
}

/**
 * @brief Get stored multi-dimension data by seq
 * @param seq Object seqence number
 * @return Data<float, float, float>
 */
Data<float, float, float> *FileReader::multiDimensionData(int seq)
{
    if (m_multiDimensionData.contains(seq)) {
        return m_multiDimensionData.value(seq);
    }
    else {
        return NULL;
    }
}
