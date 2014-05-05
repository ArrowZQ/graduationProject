/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Data Lib
** File: fileio.h
** Description: FileIO.
**
** Created By: wangshuo, wangshuo@xdt-inc.com
** Created Date: 2014-1-15
**
***************************************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <QVector>
#include <QList>
#include <QString>
#include <QObject>
#include <QHash>

#include "io.h"

namespace ALP {

template<typename T1,typename T2, typename T3>
/**
 * @brief The Data class
 *
 * Data class is used to store all kinds of data extracted from data file, including:
 *  - Curve
 *  - Wave
 *  - FMT
 *  - TDT
 *  - Table
 *  - Stream
 */
class DATASHARED_EXPORT Data
{
public:
//    QList<QPair<T1,int>*> Depths;
//    QList<QPair<T2,T3>*> TimeValue;
    QMap<T1,QList<QPair<T2,T3>*>*> data;

/*
    // Curve
    T1 - Depth
    T2 - Time
    T3 - Value
*/
};

/**
 * @brief The MetaData class
 *
 * Meta class is used to store all kinds of metadata related to data.
 */
class DATASHARED_EXPORT MetaData : public QObject
{
    Q_OBJECT

public:
/*
    // common
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString aliasName READ aliasName WRITE setAliasName)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)
    Q_PROPERTY(QString aliasUnit READ aliasUnit WRITE setAliasUnit)
    Q_PROPERTY(int type READ type WRITE setType)
    Q_PROPERTY(int typeLenth READ typeLenth WRITE setTypeLenth)
    Q_PROPERTY(float minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(float maxValue READ maxValue WRITE setMaxValue)

    // depth
    Q_PROPERTY(float startDepth READ startDepth WRITE setStartDepth)
    Q_PROPERTY(float endDepth READ endDepth WRITE setEndDepth)
    Q_PROPERTY(int depthSample READ depthSample WRITE setDepthSample)
    Q_PROPERTY(int depthInterval READ depthInterval WRITE setDepthInterval)
    Q_PROPERTY(QString depthUnit READ depthUnit WRITE setDepthUnit)

    // time
    Q_PROPERTY(float startTime READ startTime WRITE setStartTime)
    Q_PROPERTY(float endTime READ endTime WRITE setEndTime)
    Q_PROPERTY(int timeSample READ timeSample WRITE setTimeSample)
    Q_PROPERTY(int maxTimeSample READ maxTimeSample WRITE setMaxTimeSample)
    Q_PROPERTY(int timeInterval READ timeInterval WRITE setTimeInterval)
    Q_PROPERTY(QString timeUnit READ timeUnit WRITE setTimeUnit)

    // logging
    Q_PROPERTY(int logCount READ logCount WRITE setLogCount)
    Q_PROPERTY(int maxLogSample READ maxLogSample WRITE setMaxLogSample)
    Q_PROPERTY(int logTimeUnit READ logTimeUnit WRITE setTotalLog)

    // table
    Q_PROPERTY(int recordCount READ recordCount WRITE setRecordCount)
    Q_PROPERTY(int fieldCount READ fieldCount WRITE setFieldCount)

    // stream
    Q_PROPERTY(int objectLength READ objectLength WRITE setObjectLength)

*/

public:
    explicit MetaData(QObject *parent = 0);
};


/**
 * @brief File Read/Write Class
 *
 * FileIO provides fundmental functions to read/write logging data files
 *
 * @code
 * #inclde <fileio.h>
 *
 * FilIO *fileIO = new FileIO(QDir::homePath() + "/alp/Demo.ALP");
 *
 * fileIO->list();
 * @endcode
 *
 * @author wangshuo
 * @author Micle Bu
 * @date 2014.1.15
 */
class DATASHARED_EXPORT FileIO : public IO
{
public:
    /**
     * @brief ObjectAttribute
     */
    enum ObjectAttribute {
        ChannelAttribure    = 1,
        TableAttribure      = 2,
        StreamAttribure     = 3
    };

    /**
     * @brief ObjectType
     */
    enum ObjectType {
        AllObject           = 0,
        CurveObject         = 1,
        WaveObject          = 2,
        FMTObject           = 3,
        TDTObject           = 4,
        TableObject         = 5,
        StreamObject        = 6,
        ConstObject         = 7,

        // Always at last
        NoneObject          = 99
    };

    /**
     * @brief DataType
     */
    enum DataType {
        CharData            = 1,
        ShortData           = 2,
        LongData            = 3,
        FloatData           = 4,
        DoubleData          = 5,
        UnsignedCharData    = 6,
        UnsignedShortData   = 7,
        UnsignedLongData    = 8
    };


    enum StreamSubAttr {
        templateInfo        =1,
        projectInfo         =2,
        image               =3
    };

    enum ObjectStatus {
        normal              =0,
        discard             =1,
        del                 =2
    };

    enum FileType {
        ALPFile   =1,
        AXPFile   =2,

        // always the last one
        NoneFile  =99
    };

public:
    FileIO();
    ~FileIO();

    virtual bool initialization();
    virtual void *list(int type = 0);
    virtual int type(int seq) = 0;
    virtual void openFile(const QString &fileName) = 0;
    virtual MetaData *meta(int seq);
    virtual void *data(int seq);

    virtual bool updateName(int seq, QString name);
    virtual bool writeChannelData();
    virtual bool writeTableData();
    virtual bool writeStreamData();


    QVector<float> *depths(float start, float interval, int number);

    bool isRead() const;
    void setRead(bool isRead);

    QString fileAbsoluteName() const;
    void setFileAbsoluteName(const QString &fileAbsoluteName);

    bool isInit() const;
    void setInit(bool init);

    bool isOpen() const;
    void setOpen(bool isOpen);

protected:
    int objectType(int attribute, int subattribute);

protected:
    void open(const QString &fileName);
    std::ifstream m_fileStream;
    std::ofstream m_fileWriteStream;
    bool m_isOpen;
    bool m_isInit;
    bool m_isRead;
    QString m_fileAbsoluteName;
};

}

#endif // FILEIO_H
