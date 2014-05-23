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
#include "alpdata.h"
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
    bool createNewAlpFile(const QString &fileName);
    virtual bool writeChannelData(ALP_OBJECT_ENTRY *entry ,ALP_CHANNEL *channel, Data<float,float,float> *data);
    virtual bool writeTableData(ALP_OBJECT_ENTRY *entry, QList<ALP_TABLE_FIELD *> listTableField, QList<QPair<int, QList<void *> > > recoders);
    virtual bool writeStreamData(ALP_OBJECT_ENTRY *entry, const char *buf);
    virtual bool updateName(int seq, QString name);


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
