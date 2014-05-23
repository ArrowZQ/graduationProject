/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: alpdata.h
** Description: AlpData.
**
** Created By: tang, tang@xdt-inc.com
** Created Date: 2013-10-12T20:42:35
**
***************************************************************************************************/

#ifndef ALPDATA_H
#define ALPDATA_H
#define MAX_SAMPLES 1000
#include <QDebug>
#include <QObject>
#include "data_global.h"

//======================================= Section: ALP File Data Structure - start ===========================================
/**
 * ALP Head
 */
typedef struct tagALP_HEAD
{
    short   MachineType;                   //机器类型：0-PC,1-SUN,2-IBM,3-HP -1-初始化默认
    short   MaxObjectNumber;               //允许记录的最大对象数，默认为512，可在文件产生时给出
    short   ObjectNumber;                  //当前记录中对象的数目(包括删除和抛弃的对象)
//    short   BlockLen;                      //块的长度（字节数）
    int     EntryOffsetFirst;              //第一个对象入口记录从文件开始的偏移量
//    int     EntryOffsetLast;               //最后一个对象入口记录从文件开始的偏移量
    int     FileSize;                      //文件的字节数
    int      TimeCreate;                   //文件产生的时间
    char   Reserved[32];                   //保留字节
}ALP_HEAD;

/**
 * ALP Object meta
 */
typedef struct tagALP_OBJECT_ENTRY
{
    char   Name[16];                      //对象名
    int    Status;                        //对象状态:0-正常,1:抛弃,2:删除
    short  Attribute;                     //对象主属性:1-通道对象,2-表对象,3-流对象
    short  SubAttribute;                  //对象子属性
    int  Position;                        //对象数据体从文件开始处的偏移量
//    int  BlockNum;                        //对象数据体占用磁盘的块数
    int  nextEntryPos;                    //下一个对象如果记录从文件开始处的偏移量
    int  dataLen;                         //对象数据体的长度
    int  TimeCreate;                      //对象产生的时间
    int  TimeWrite;                       //对象最近修改的时间
    char   Reserved[32];                  //保留字节
}ALP_OBJECT_ENTRY;                        //对象入口结构

/**
 * ALP Dimension meta
 */
typedef struct tagALP_CHANNEL_DIMENSION
{
    char   Name[8];                       //维名称
    char   Unit[8];                       //维单位
    char   AliasName[16];                 //维别名
    float  StartVal;                      //维的开始值
    float  Delta;                         //维的采集或计算增量
    int  Samples;                       //维的数据采样点数
    int  MaxSamples;                    //维的数据采样的最大点数
    int  Size;                          //该维上每一采样点所占用的字节数
    short   RepCode;                       //维的数据类型
    short   Reserved;                      //保留字节
}ALP_CHANNEL_DIMENSION;                   //通道维信息结构

/**
 * ALP Channel meta
 */
typedef struct tagALP_CHANNEL
{
    char   Unit[8];                       //对象单位
    char   AliasName[16];                 //对象别名
    char   AliasUnit[16];                 //单位别名
    short   RepCode;                       //对象数据类型
    short   CodeLen;                       //数据类型长度
    float  MinVal;                        //对象最小值:缺省左刻度值
    float  MaxVal;                        //对象最大值：缺省右刻度值
    short   Reserved;                      //保留字节
    short   NumOfDimension;                //对象维信息数
    ALP_CHANNEL_DIMENSION DimInfo[4];     //对象维信息
}ALP_CHANNEL;                             //通道信息结构

//------------------------------ REGION: MetaData ------------------------------
/**
 * Stream object meta
 */
typedef struct tagALP_STREAM
{
    int  Length;                  //对象长度
}ALP_STREAM;                              //流对象结构

/**
 * Table object meta
 */
typedef struct tagALP_TABLE_FIELD
{
    char   Name[32];                      //字段名称
    short   RepCode;                       //字段值的浮点类型
    short   Length;                        //字段值的长度
    int  Reserved;                      //保留字节
}ALP_TABLE_FIELD;                         //字段信息结构

typedef struct tagALP_TABLE
{
    int  RecordCount;                   //表的记录数
    int  FieldCount;                    //表的字段数
}ALP_TABLE;                               //表信息结构



/**
 * @brief The Table class
 *
 * One record in table
 *
 */
class DATASHARED_EXPORT Table : public QObject
{
    Q_OBJECT

public:
    explicit Table(QObject *parent = 0);
};

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
    explicit MetaData(QObject *parent = 0);
};



class AlpData
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


public:
    AlpData();

    //数据结构装载函数
    static ALP_OBJECT_ENTRY *entry(QString name, ObjectStatus status, ObjectAttribute attr, ObjectType subattr);
    static ALP_CHANNEL *channel(QString unit, QString aName, QString aUnit,DataType datatype,
                                ALP_CHANNEL_DIMENSION* oneDim, ALP_CHANNEL_DIMENSION* twoDim = NULL,
                                ALP_CHANNEL_DIMENSION* threeDim = NULL, ALP_CHANNEL_DIMENSION* fourDim = NULL);
    static ALP_CHANNEL_DIMENSION *channelDim(QString name, QString unit, QString aName,float delta, DataType datatype);
    static ALP_STREAM *stream();
    static ALP_TABLE *table();
    static ALP_TABLE_FIELD *tableField();

private:
    static int dataTypeLen(DataType datatype);
};

#endif // ALPDATA_H
