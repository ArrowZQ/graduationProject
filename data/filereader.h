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

#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include "alpio.h"
#include "fileshare.h"
#include <QPolygonF>
#include <QDebug>

namespace ALP {

class DataListItem {
public:
    QString Name;
    int Type;
    int Seq;
};

/**
 * @brief The FileReader class. 测井数据解析封装类
 *
 *  - Open and manage data files.
 *  - Extract and get list or meta or data of object.
 *
 * @code
 * #include <filereader.h>
 *
 * FileReader reader;
 * if (reader.open(fileName)) {
 *     QVector<DataListItem> allList = reader.list();
 *     MetaData *meta = reader.meta(seq);
 *     void *data = reader.meta(seq);
 *     QPolygonF curveData = reader.curveData(seq);
 * }
 * @endcode
 *
 * @author Micle Bu
 * @date 2014-03-15
 */
class FileReader
{
public:
    FileReader();
    ~FileReader();

    enum FileType {
        ALPFile,
        WISFile,
        AXPFile,

        // always the last one
        NoneFile
    };

    bool open(const QString &fileName);
    QVector<DataListItem> list(int type = 0);
    MetaData *meta(int seq);
    MetaData *meta(const QString &name);
    void *data(int seq);
    void *data(const QString &name);

    QPolygonF curveData(int seq);
    QPolygonF curveData(const QString &name);

    QPolygonF arrayData(int seq, int &count, int index = 0);
    QPolygonF arrayData(const QString &name, int &count, int index = 0);

    QList<QPair<float, QList<float> > > waveData(int seq);
    QList<QPair<float, QList<float> > > waveData(const QString &name);


private:
    FileIO *m_fileIO;
    QString m_fileName;
    int m_fileType;
    bool m_isOpen;

    QMap<int, Data<float,float,float> *> m_multiDimensionData;

private:
    int fileType(const QString &fileName);
    int objectSequence(const QString &name);
    int objectType(int attribute, int subattribute);

    void clearData();
    Data<float,float,float> *multiDimensionData(int seq);
};

}

#endif // FILEREADER_H
