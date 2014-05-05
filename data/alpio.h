/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Data Lib
** File: alpio.h
** Description: AlpIO.
**
** Created By: wangshuo, wangshuo@xdt-inc.com
** Created Date: 2014-1-15
** Modified By: Micle Bu, 2014-03-20
** Modified Note: Update alp decoder
**
***************************************************************************************************/

#ifndef ALPIO_H
#define ALPIO_H

#include "fileio.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <QList>
#include <QObject>
#include <QDate>

namespace ALP{

#define REPR_INT     	1
#define REPR_SHORT   	2
#define REPR_LONG    	3
#define REPR_FLOAT   	4
#define REPR_DOUBLE  	5
#define REPR_STRING  	6
#define REPR_CHAR    	7

#define REPR_UCHAR   	8
#define REPR_USHORT  	9
#define REPR_UINT   	10
#define REPR_ULONG  	11

#define CHANNEL_OBJECT  1
#define TABLE_OBJECT    2
#define STREAM_OBJECT   3

#define CURVE_OBJECT    1
#define WAVE_OBJECT     2
#define FMT_OBJECT      3
#define TDT_OBJECT      4

#define ROW 655350
#define COL 512

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
 * Curve object meta
 */
typedef struct tagALP_CURVE
{
    char Name[16];                        //对象名称
    char AliasName[16];                   //对象别名
    char Unit[8];                         //对象单位
    char AliasUnit[16];                   //对象单位别名
    short RepCode;                         //对象数据代码类型
    short CodeLen;                         //对象数据代码长度
    float MinValue;                       //对象最小值(左刻度值)
    float MaxValue;                       //对象最大值(左刻度值)
    int DepthNum;                         //深度采样点数
    float StartDepth;                     //开始深度
    float EndDepth;                       //结束深度
    float DepLevel;                       //深度采样间隔
    char  DepthUnit[8];                   //深度单位
}ALP_CURVE;

/**
 * Wave object meta
 */
typedef struct tagALP_WAVE
{
    char Name[16];                        //对象名称
    char AliasName[16];                   //对象别名
    char Unit[8];                         //对象单位
    char AliasUnit[16];                   //对象单位别名
    short RepCode;                         //对象数据代码类型
    short CodeLen;                         //对象数据代码长度
    float MinValue;                       //对象最小值(左刻度值)
    float MaxValue;                       //对象最大值(左刻度值)
    int DepthSamples;                     //深度采样点数
    float StartDepth;                     //开始深度
    float EndDepth;                       //结束深度
    float DepLevel;                       //深度采样间隔
    char DepthUnit[8];                    //深度单位
    int TimeSamples;                      //时间采样点数
    float StartTime;                      //开始时间
    float EndTime;                        //结束时间
    float TimeLevel;                      //时间采样间隔
    char TimeUnit[8];                     //时间单位
}ALP_WAVE;

/**
 * FMT object meta
 */
typedef struct tagALP_FMT
{
    char Name[16];                        //对象名称
    char AliasName[16];                   //对象别名
    char Unit[8];                         //对象单位
    char AliasUnit[16];                   //对象单位别名
    short RepCode;                         //对象代码类型
    short CodeLen;                         //对象代码长度
    float MinValue;                       //对象最小值(左刻度值)
    float MaxValue;                       //对象最大值(左刻度值)
    int TotalPointNum;                    //总深度点数
    int MaxPointSamples;                  //最大的时间采样点数
    char DepthUnit[8];                    //深度单位
    float StartTime;                      //开始时间
    float TimeLevel;                      //时间采样间隔
    char TimeUnit[8];                     //时间单位
}ALP_FMT;

/**
 * TDT object meta
 */
typedef struct tagALP_TDT
{
    char Name[16];                        //对象名称
    char AliasName[16];                   //对象别名
    char Unit[8];                         //对象单位
    char AliasUnit[16];                   //单位别名
    short RepCode;                         //对象代码类型
    short CodeLen;                         //对象代码长度
    float MinValue;                       //对象最小值(左刻度值)
    float MaxValue;                       //对象最大值(左刻度值)
    int TotalLogNumber;                 //总测井次数
    int MaxPointSamples;                //最大的一次测井采样点数
    char LogTimeUnit[8];                  //测井时间单位
    char DepthUnit[8];                    //深度单位
    char TimeUnit[8];                     //采样时间单位
}ALP_TDT;

/**
 * TDT object meta
 */
typedef struct tagALP_TDT_DATA
{
    float Depth;                          //采样点的深度
    float Time;                           //采样时间
    float Value;                          //采样值
}ALP_TDT_DATA;

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


////old
//typedef struct tagCURVE_HEAD
//{
//    char   Name[16];                    // 曲线的名字
//    char   Unit[8];                     // 曲线的单位
//    float  startMD;                     // 曲线的起始深度
//    float  stopMD;                      // 曲线的结束深度
//    int    pointNum;                    // 数据样点数
//    int    position;                    // 数据位置
//} CURVE_HEAD;                           // 曲线信息结构

////old
//typedef struct tagCURVE_DATA
//{
//    char   Name[16];                    // 曲线的名字
//    char   Unit[8];                     // 曲线的单位
//    float  Depth[ROW];                  // 曲线的深度值
//    float  Value[ROW];                  // 曲线的值
//    int    Count;                       // 曲线测量值计数
//} CURVE_DATA;                           // 曲线数据结构

//======================================= Section: ALP File Data Structure - end ===========================================




//======================================= Section: AlpIO ===========================================
/**
 * @brief ALP File Encoder and Decoder Class. ALP文件读写类。
 *
 * @author wangshuo
 * @date 2014.1.15
 */
class DATASHARED_EXPORT AlpIO : public FileIO
{
public:
    AlpIO();
    ~AlpIO();

    //new
    void openFile(const QString &fileName);
    virtual MetaData *meta(int seq);
    virtual void *data(int seq);
    virtual QList<ALP_OBJECT_ENTRY *> *list(int type = 0);

    //write file
    virtual bool writeChannelData();
    virtual bool writeTableData();
    virtual bool writeStreamData();

    bool createNewAlpFile(const QString &fileName);
    bool writeChannel(ALP_OBJECT_ENTRY *entry ,ALP_CHANNEL *channel, Data<float,float,float> *data);
    bool writeTable(ALP_OBJECT_ENTRY *entry, QList<ALP_TABLE_FIELD *> listTableField, QList<QPair<int, QList<void *> > > recoders);
    bool writeStream(ALP_OBJECT_ENTRY *entry, const char *buf);
    bool updateName(int seq, QString name);

    static ALP_OBJECT_ENTRY *entry(QString name, FileIO::ObjectAttribute attr, FileIO::ObjectType subattr);
    static ALP_CHANNEL *channel(QString unit, QString aName, QString aUnit, QList<ALP_CHANNEL_DIMENSION> dim);
    static ALP_CHANNEL_DIMENSION *channelDim(QString name, QString unit, QString aName,float delta, int MaxSample);
    static ALP_STREAM *stream();
    static ALP_TABLE *table();
    static ALP_TABLE_FIELD *tableField();

private:
    virtual bool initialization();
    virtual int type(int seq);

    // meta
    void *curvesMeta(ALP_OBJECT_ENTRY *entry);
    void *waveMeta(ALP_OBJECT_ENTRY *entry);
    void *fmtMeta(ALP_OBJECT_ENTRY *entry);
    void *tdtMeta(ALP_OBJECT_ENTRY *entry);
    void *tableMeta(ALP_OBJECT_ENTRY *entry);
    void *tableMeta(QString name);
    void *streamMeta(ALP_OBJECT_ENTRY *entry);

    // data
    void *curveData(ALP_OBJECT_ENTRY *entry);
    void *waveData(ALP_OBJECT_ENTRY *entry);
    void *fmtData(ALP_OBJECT_ENTRY *entry);
    void *tdtData(ALP_OBJECT_ENTRY *entry);
    void *tableData(ALP_OBJECT_ENTRY *entry);
    void *tableData(QString name);
    void *streamData(ALP_OBJECT_ENTRY *entry);
    void *constData(QString name);

    // datatype
    void *data(void *meta, int datatype, int objectType);
    void *dataChar(void *info, int type);
    void *dataShort(void *info, int type);
    void *dataLong(void *info, int type);
    void *dataFloat(void *info, int type);
    void *dataDouble(void *info, int type);
    void *dataUnsignedChar(void *info, int type);
    void *dataUnsignedShort(void *info, int type);
    void *dataUnsignedLong(void *info, int type);

    //write
    void updateHead();
    void updateEntry(ALP_OBJECT_ENTRY *entry);

private:
//    std::ifstream m_alpFile;
    ALP_HEAD *m_head;

    QList<ALP_OBJECT_ENTRY *> *m_allList;
    QList<ALP_OBJECT_ENTRY *> *m_curveList;     //曲线对象入口list
    QList<ALP_OBJECT_ENTRY *> *m_waveList;      //波形对象入口list
    QList<ALP_OBJECT_ENTRY *> *m_fmtList;       //地层测试对象入口list
    QList<ALP_OBJECT_ENTRY *> *m_tdtList;       //时深时对象入口list
    QList<ALP_OBJECT_ENTRY *> *m_tableList;
    QList<ALP_OBJECT_ENTRY *> *m_streamList;
    QList<ALP_TABLE_FIELD *>  *m_alpTableField;
};

}
#endif // ALPIO_H
