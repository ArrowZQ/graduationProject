/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: alpio.cpp
** Description: AlpIO.
**
** Created By: wangshuo, wangshuo@xdt-inc.com
** Created Date: 2013-10-12T20:42:35
**
***************************************************************************************************/

#include "alpio.h"

#include <QString>
#include <string>
#include <QStringList>
#include <QDebug>

using namespace ALP;

/**
 * @brief 构造函数，用于打开ALP文件得到文件句柄
 * @param filename ALP文件名
 */
AlpIO::AlpIO() :
    FileIO()
{
//    // Update
//    m_isInit = false;

//    // new
//    m_head = new ALP_HEAD;
//    m_curveList = new QList<ALP_OBJECT_ENTRY*>();
//    m_waveList = new QList<ALP_OBJECT_ENTRY*>();
//    m_fmtList = new QList<ALP_OBJECT_ENTRY*>();
//    m_tdtList = new QList<ALP_OBJECT_ENTRY*>();
//    m_tableList = new QList<ALP_OBJECT_ENTRY*>();
//    m_streamList = new QList<ALP_OBJECT_ENTRY*>();
//    m_allList = new QList<ALP_OBJECT_ENTRY*>();
//    m_alpTableField = new QList<ALP_TABLE_FIELD*>();

//    // Open file
//    std::string file = std::string((const char *)fileName.toLocal8Bit());
//    m_fileStream.open(file.c_str(), std::ios_base::binary);

//    // Set file open status
//    setOpen(m_fileStream.is_open());
}

/**
 * @brief 析构函数
 */
AlpIO::~AlpIO()
{
    delete m_head;
    delete m_curveList;
    delete m_waveList;
    delete m_fmtList;
    delete m_tdtList;
    delete m_tableList;
    delete m_streamList;
    delete m_allList;
    delete m_alpTableField;
}

void AlpIO::openFile(const QString &fileName)
{
    open(fileName);
    // Update
    m_isInit = false;

    // new
    m_head = new ALP_HEAD;
    m_curveList = new QList<ALP_OBJECT_ENTRY*>();
    m_waveList = new QList<ALP_OBJECT_ENTRY*>();
    m_fmtList = new QList<ALP_OBJECT_ENTRY*>();
    m_tdtList = new QList<ALP_OBJECT_ENTRY*>();
    m_tableList = new QList<ALP_OBJECT_ENTRY*>();
    m_streamList = new QList<ALP_OBJECT_ENTRY*>();
    m_allList = new QList<ALP_OBJECT_ENTRY*>();
    m_alpTableField = new QList<ALP_TABLE_FIELD*>();

}

//======================================= Section: Public methods ===========================================
/**
 * @brief Load minimum requied data to memory
 */
bool AlpIO::initialization()
{
    if (!isOpen()) {
        return false;
    }

    // 读取文件头结构，得到对象入口记录相对于文件起始位置的偏移量
    m_fileStream.seekg(10, std::ios_base::beg);
    m_fileStream.read(reinterpret_cast<char *>(m_head), sizeof(ALP_HEAD));
//    m_fileStream.seekg(m_head->EntryOffsetFirst, std::ios_base::beg);

    int objectNum = m_head->ObjectNumber;

    // 遍历ALP文件中的对象入口记录，通过对象的属性来区分对象类别：1.通道 2.表 3.流
    for(int i = 0; i < objectNum; i++)
    {
        ALP_OBJECT_ENTRY *objectEntry = new ALP_OBJECT_ENTRY;
        m_fileStream.read(reinterpret_cast<char *>(objectEntry), sizeof(ALP_OBJECT_ENTRY));

        if(objectEntry->Attribute == 0)
        {
            break;
        }
        else if(objectEntry->Attribute == ChannelAttribure)
        {
            if(objectEntry->SubAttribute == CurveObject)
                m_curveList->append(objectEntry);
            else if(objectEntry->SubAttribute == WaveObject)
                m_waveList->append(objectEntry);
            else if(objectEntry->SubAttribute == FMTObject)
                m_fmtList->append(objectEntry);
            else if(objectEntry->SubAttribute == TDTObject)
                m_tdtList->append(objectEntry);
        }
        else if(objectEntry->Attribute == TableAttribure) {
            m_tableList->append(objectEntry);
        }
        else if(objectEntry->Attribute == StreamAttribure) {
            m_streamList->append(objectEntry);
        }

        m_fileStream.seekg(objectEntry->nextEntryPos, std::ios_base::beg);
        m_allList->append(objectEntry);
    }
    setInit(true);

    return true;
}

/**
 * @brief Get object list
 * @param type Object type
 * @return
 */
QList<ALP_OBJECT_ENTRY *> *AlpIO::list(int type)
{
    // Always use data after opening and initialization
    if (!isInit() && !initialization()) {
        return NULL;
    }

    switch (type) {
    case AllObject:
        return m_allList;
    case CurveObject:
        return m_curveList;
    case WaveObject:
        return m_waveList;
    case FMTObject:
        return m_fmtList;
    case TDTObject:
        return m_tdtList;
    case TableObject:
        return m_tableList;
    case StreamObject:
        return m_streamList;
    default:
        return NULL;
    }
}

int AlpIO::type(int seq)
{
    // Always use data after opening and initialization
    if (!isInit() && !initialization()) {
        return NoneObject;
    }

    // Locate the object
    for (int i = 0; i < m_allList->count(); i++) {
        ALP_OBJECT_ENTRY *entry = m_allList->at(i);

        if (entry->Position == seq) {
            return objectType(entry->Attribute, entry->SubAttribute);
        }
    }

    return NoneObject;
}


/**
 * @brief Get object data be seq
 * @param seq Sequence number of the object in data files
 * @return
 */
MetaData *AlpIO::meta(int seq)
{
    MetaData *meta = new MetaData;

    // Always use data after opening and initialization
    if (!isInit() && !initialization()) {
        return meta;
    }

    // Locate the object
    for (int i = 0; i < m_allList->count(); i++) {
        ALP_OBJECT_ENTRY *entry = m_allList->at(i);

        if (entry->Position == seq) {
            int type = objectType(entry->Attribute, entry->SubAttribute);

            // Locate the object
            switch (type) {
            case CurveObject: {
                ALP_CURVE *d = static_cast<ALP_CURVE *>(curvesMeta(entry));

                if (d) {
                    meta->setProperty("name", d->Name);
                    meta->setProperty("aliasName", d->AliasName);
                    meta->setProperty("unit", d->Unit);
                    meta->setProperty("aliasUnit", d->AliasUnit);
                    meta->setProperty("type", d->RepCode);
                    meta->setProperty("typeLength", d->CodeLen);
                    meta->setProperty("minValue", d->MinValue);
                    meta->setProperty("maxValue", d->MaxValue);
                    meta->setProperty("startDepth", d->StartDepth);
                    meta->setProperty("endDepth", d->EndDepth);
                    meta->setProperty("depthSample", d->DepthNum);
                    meta->setProperty("depthInterval", d->DepLevel);
                    meta->setProperty("depthUnit", d->DepthUnit);
                }
                break;
            }
            case WaveObject: {
                ALP_WAVE *d = static_cast<ALP_WAVE *>(waveMeta(entry));

                if (d) {
                    meta->setProperty("name", d->Name);
                    meta->setProperty("aliasName", d->AliasName);
                    meta->setProperty("unit", d->Unit);
                    meta->setProperty("aliasUnit", d->AliasUnit);
                    meta->setProperty("type", d->RepCode);
                    meta->setProperty("typeLength", d->CodeLen);
                    meta->setProperty("minValue", d->MinValue);
                    meta->setProperty("maxValue", d->MaxValue);
                    meta->setProperty("startDepth", d->StartDepth);
                    meta->setProperty("endDepth", d->EndDepth);
                    meta->setProperty("depthSample", d->DepthSamples);
                    meta->setProperty("depthInterval", d->DepLevel);
                    meta->setProperty("depthUnit", d->DepthUnit);
                    meta->setProperty("startTime", d->StartTime);
                    meta->setProperty("endTime", d->EndTime);
                    meta->setProperty("timeSample", d->TimeSamples);
                    meta->setProperty("timeInterval", d->TimeLevel);
                    meta->setProperty("timeUnit", d->TimeUnit);
                }
                break;
            }
            case FMTObject: {
                ALP_FMT *d = static_cast<ALP_FMT *>(fmtMeta(entry));

                if (d) {
                    meta->setProperty("name", d->Name);
                    meta->setProperty("aliasName", d->AliasName);
                    meta->setProperty("unit", d->Unit);
                    meta->setProperty("aliasUnit", d->AliasUnit);
                    meta->setProperty("type", d->RepCode);
                    meta->setProperty("typeLength", d->CodeLen);
                    meta->setProperty("minValue", d->MinValue);
                    meta->setProperty("maxValue", d->MaxValue);
                    meta->setProperty("depthSample", d->TotalPointNum);
                    meta->setProperty("depthUnit", d->DepthUnit);
                    meta->setProperty("maxTimeSample", d->MaxPointSamples);
                    meta->setProperty("startTime", d->StartTime);
                    meta->setProperty("timeUnit", d->TimeUnit);
                    meta->setProperty("timeInterval", d->TimeLevel);
                }
                break;
            }
            case TDTObject: {
                ALP_TDT *d = static_cast<ALP_TDT *>(tdtMeta(entry));

                if (d) {
                    meta->setProperty("name", d->Name);
                    meta->setProperty("aliasName", d->AliasName);
                    meta->setProperty("unit", d->Unit);
                    meta->setProperty("aliasUnit", d->AliasUnit);
                    meta->setProperty("type", d->RepCode);
                    meta->setProperty("typeLength", d->CodeLen);
                    meta->setProperty("minValue", d->MinValue);
                    meta->setProperty("maxValue", d->MaxValue);
                    meta->setProperty("logCount", d->TotalLogNumber);
                    meta->setProperty("maxLogSample", d->MaxPointSamples);
                    meta->setProperty("logTimeUnit", d->LogTimeUnit);
                    meta->setProperty("depthUnit", d->DepthUnit);
                    meta->setProperty("timeUnit", d->TimeUnit);
                }
                break;
            }
            case TableObject: {
                ALP_TABLE *d = static_cast<ALP_TABLE *>(tableMeta(entry));

                if (d) {
                    meta->setProperty("recordCount", d->RecordCount);
                    meta->setProperty("fieldCount", d->FieldCount);
                }
                break;
            }
            case StreamObject: {
                ALP_STREAM *d = static_cast<ALP_STREAM *>(streamData(entry));

                if (d) {
                    meta->setProperty("objectLength", d->Length);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return meta;
}

/**
 * @brief Get object data by seq
 *  - startDepth
 *  - endDepth
 *  - depthInterval
 *  Data range: [startDepth, endDepth - depthInterval]
 * @param seq Sequence number of the object in data files
 * @return Data<T1, T2, T2>
 */
void *AlpIO::data(int seq)
{
    // Always use data after opening and initialization
    if (!isInit() && !initialization()) {
        return NULL;
    }

    // Locate the object
    for (int i = 0; i < m_allList->count(); i++) {
        ALP_OBJECT_ENTRY *entry = m_allList->at(i);

        if (entry->Position == seq) {
            int type = objectType(entry->Attribute, entry->SubAttribute);

            switch (type) {
            case CurveObject:
                return curveData(entry);
            case WaveObject:
                return waveData(entry);
            case FMTObject:
                return fmtData(entry);
            case TDTObject:
                return tdtData(entry);
            case TableObject:
                return tableData(entry);
            case StreamObject:
                return streamData(entry);
            default:
                return NULL;
            }
        }
    }

    return NULL;
}

bool AlpIO::createNewAlpFile(const QString &fileName)
{
    std::string file = std::string((const char *)fileName.toLocal8Bit());
    m_fileWriteStream.open(file.c_str(), std::ios_base::binary | std::ios_base::out);
    if (m_fileWriteStream.is_open()) {
        //创建文件并且写入文件标识
        const char *alpFileCode = "ALP1.0";
        m_fileWriteStream.write(alpFileCode, 10);

        //写入文件头结构
        ALP_HEAD *head = new ALP_HEAD;
        head->MachineType = -1;
        head->MaxObjectNumber = 512;
        head->ObjectNumber = 0;
        head->EntryOffsetFirst = sizeof(ALP_HEAD) + 10;
//        head->EntryOffsetLast = sizeof(ALP_HEAD) + 10;
        head->FileSize = sizeof(ALP_HEAD) + 10;
        head->TimeCreate = QDate::currentDate().toJulianDay();
        m_fileWriteStream.write((char *)head, sizeof(ALP_HEAD));

        m_fileWriteStream.close();
        return true;
    } else {
        return false;
    }
}

bool AlpIO::writeChannel(ALP_OBJECT_ENTRY *entry, ALP_CHANNEL *channel, Data<float,float,float> *data)
{
    // Always use data after opening and initialization
    if (!isInit() && !initialization()) {
        return false;
    }

    //对象入口
    m_fileWriteStream.seekp(std::ios_base::end);
    qDebug()<<m_fileWriteStream.tellp();
    m_fileWriteStream.seekp(m_head->FileSize ,std::ios_base::beg);
    qDebug()<<m_fileWriteStream.tellp();
    //改变通道对象
    entry->Position = m_head->FileSize + sizeof(ALP_OBJECT_ENTRY);
    entry->TimeCreate = QDate::currentDate().toJulianDay();
    entry->TimeWrite = QDate::currentDate().toJulianDay();

    QMap<float,QList<QPair<float,float>*>*> dataReal = data->data;
    if (entry->SubAttribute == CurveObject) {
        if (channel->DimInfo[0].Delta == 0) {
            //写入对象入口和通道对象
            int datalen = dataReal.size() * sizeof(float) * 2 + sizeof(ALP_CHANNEL);
            entry->nextEntryPos = entry->Position + datalen;
            entry->dataLen = datalen;
            m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
            m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

            QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
            while (i != dataReal.constEnd()) {
                m_fileWriteStream.write((char *)&i.key(), sizeof(float));
                m_fileWriteStream.write((char *)&i.value()->first()->second, sizeof(float));
                ++i;
            }
        } else {
            int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
            entry->nextEntryPos = entry->Position + datalen;
            entry->dataLen = datalen;
            m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
            m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

            QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
            while (i != dataReal.constEnd()) {
                m_fileWriteStream.write((char *)&i.value()->first()->second, sizeof(float));
                ++i;
            }
        }
        m_head->ObjectNumber++;
        m_head->FileSize = entry->nextEntryPos;
        updateHead();
        return true;
    } else if (entry->SubAttribute == WaveObject) {
        if (channel->DimInfo[0].Delta == 0) {
            if (channel->DimInfo[1].Delta == 0) {
                if (channel->DimInfo[1].Samples == 0) {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        m_fileWriteStream.write((char *)&i.key(), sizeof(float));  //写入深度
                        datalen += sizeof(float);

                        QList<QPair<float,float>*> *listpair = i.value();
                        m_fileWriteStream.write((char *)listpair->size(), sizeof(float));  //写入采样点数
                        datalen += sizeof(float);

                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->first, sizeof(float));
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float) * 2;

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                } else {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        m_fileWriteStream.write((char *)&i.key(), sizeof(float));  //写入深度
                        datalen += sizeof(float);

                        QList<QPair<float,float>*> *listpair = i.value();
                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->first, sizeof(float));
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float) * 2;

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                }
            } else {
                if (channel->DimInfo[1].Samples == 0) {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        m_fileWriteStream.write((char *)&i.key(), sizeof(float));  //写入深度
                        datalen += sizeof(float);

                        QList<QPair<float,float>*> *listpair = i.value();
                        m_fileWriteStream.write((char *)listpair->size(), sizeof(float));  //写入采样点数
                        datalen += sizeof(float);

                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float);

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                } else {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        m_fileWriteStream.write((char *)&i.key(), sizeof(float));  //写入深度
                        datalen += sizeof(float);

                        QList<QPair<float,float>*> *listpair = i.value();
                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float);

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                }
            }
        } else {
            if (channel->DimInfo[1].Delta == 0) {
                if (channel->DimInfo[1].Samples == 0) {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        QList<QPair<float,float>*> *listpair = i.value();
                        m_fileWriteStream.write((char *)listpair->size(), sizeof(float));  //写入采样点数
                        datalen += sizeof(float);

                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->first, sizeof(float));
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float) * 2;

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                } else {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        QList<QPair<float,float>*> *listpair = i.value();
                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->first, sizeof(float));
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float) * 2;

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                }
            } else {
                if (channel->DimInfo[1].Samples == 0) {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        QList<QPair<float,float>*> *listpair = i.value();
                        m_fileWriteStream.write((char *)listpair->size(), sizeof(float));  //写入采样点数
                        datalen += sizeof(float);

                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float);

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                } else {
                    int datalen = dataReal.size() * sizeof(float) + sizeof(ALP_CHANNEL);
                    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
                    m_fileWriteStream.write((char *)channel, sizeof(ALP_CHANNEL));

                    QMap<float,QList<QPair<float,float>*>*>::const_iterator i = dataReal.constBegin();
                    while (i != dataReal.constEnd()) {
                        QList<QPair<float,float>*> *listpair = i.value();
                        for (int k = 0; k < listpair->size(); k++) {
                            m_fileWriteStream.write((char *)&listpair->at(k)->second, sizeof(float));
                        }
                        datalen += listpair->size() * sizeof(float);

                        ++i;
                    }

                    entry->dataLen = datalen;
                    entry->nextEntryPos = entry->Position + datalen;
                    updateEntry(entry);
                }
            }
        }

        m_head->ObjectNumber++;
        m_head->FileSize = entry->nextEntryPos;
        updateHead();

        return true;
    } else if (entry->SubAttribute == FMTObject) {
        return false;
    } else if (entry->SubAttribute == TDTObject) {
        return false;
    } else {
        return false;
    }

}

bool AlpIO::writeTable(ALP_OBJECT_ENTRY *entry, QList<ALP_TABLE_FIELD *> listTableField, QList<QPair<int, QList<void *> > > recoders)
{
    // Always use data after opening and initialization
    if (!isInit() && !initialization()) {
        return false;
    }
    if (recoders.size() <= 0 || listTableField.size() != recoders.size()) {
        return false;
    }

    //对象入口
    m_fileWriteStream.seekp(std::ios_base::end);
    qDebug()<<m_fileWriteStream.tellp();
    m_fileWriteStream.seekp(m_head->FileSize ,std::ios_base::beg);
    qDebug()<<m_fileWriteStream.tellp();

    //写入通道对象
    entry->Position = m_head->FileSize + sizeof(ALP_OBJECT_ENTRY);
    entry->TimeCreate = QDate::currentDate().toJulianDay();
    entry->TimeWrite = QDate::currentDate().toJulianDay();
    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));

    int datalen = 0;
    ALP_TABLE *alptable = new ALP_TABLE;
    QList<void *> list = recoders.first().second;
    alptable->RecordCount = list.size();
    alptable->FieldCount = listTableField.size();
    m_fileWriteStream.write((char *)alptable, sizeof(ALP_TABLE));
    datalen += sizeof(ALP_TABLE);

    for (int k = 0; k < listTableField.size(); k++) {
        m_fileWriteStream.write((char *)listTableField.at(k), sizeof(ALP_TABLE_FIELD));
    }
    datalen += listTableField.size() * sizeof(ALP_TABLE_FIELD);

    for (int j = 0; j < list.size(); j++) {
        for (int m = 0; m < recoders.size(); m++) {
            QPair<int, QList<void *> > pairval = recoders.at(m);
            char *tempVal = reinterpret_cast<char *>(pairval.second.at(j));
            int dataType = pairval.first;
            if (dataType == CharData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(char));
            } else if (dataType == ShortData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(short));
            } else if (dataType == LongData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(long));
            } else if (dataType == FloatData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(float));
            } else if (dataType == DoubleData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(double));
            } else if (dataType == UnsignedCharData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(unsigned char));
            } else if (dataType == UnsignedShortData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(unsigned short));
            } else if (dataType == UnsignedLongData) {
                m_fileWriteStream.write((char *)tempVal, sizeof(unsigned long));
            } else {
                qDebug()<<"error!!!!";
            }
        }
    }
    datalen += recoders.size() * recoders.first().second.size();

    entry->dataLen = datalen;
    entry->nextEntryPos = entry->Position + datalen;
    updateEntry(entry);

    m_head->ObjectNumber++;
    m_head->FileSize = entry->nextEntryPos;
    updateHead();

    return true;
}

bool AlpIO::writeStream(ALP_OBJECT_ENTRY *entry, const char *buf)
{
    // Always use data after opening and initialization
    if (!isInit() && !initialization()) {
        return false;
    }

    //对象入口
    m_fileWriteStream.seekp(std::ios_base::end);
    qDebug()<<m_fileWriteStream.tellp();
    m_fileWriteStream.seekp(m_head->FileSize ,std::ios_base::beg);
    qDebug()<<m_fileWriteStream.tellp();

    //写入通道对象
    entry->Position = m_head->FileSize + sizeof(ALP_OBJECT_ENTRY);
    entry->TimeCreate = QDate::currentDate().toJulianDay();
    entry->TimeWrite = QDate::currentDate().toJulianDay();
    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));

    int datalen = 0;
    ALP_STREAM *stream = new ALP_STREAM;
    stream->Length = strlen(buf);
    m_fileWriteStream.write((char *)stream, sizeof(ALP_STREAM));
    datalen += sizeof(ALP_STREAM);

    m_fileWriteStream.write(buf, stream->Length);
    datalen += stream->Length;

    entry->dataLen = datalen;
    entry->nextEntryPos = entry->Position + datalen;
    updateEntry(entry);

    m_head->ObjectNumber++;
    m_head->FileSize = entry->nextEntryPos;
    updateHead();

    return true;
}

//======================================= Section: Private methods ===========================================
//------------------------------ REGION: MetaData ------------------------------
/**
 * @brief Get curve's meta
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return <ALP_CURVE> data
 */
void *AlpIO::curvesMeta(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extract meta
    ALP_CURVE *meta = new ALP_CURVE;

    m_fileStream.seekg(entry->Position, std::ios_base::beg);

    ALP_CHANNEL *channel = new ALP_CHANNEL;
    m_fileStream.read(reinterpret_cast<char *>(channel), sizeof(ALP_CHANNEL));

    meta->DepthNum = channel->DimInfo[0].Samples;
    meta->StartDepth = channel->DimInfo[0].StartVal;
    float delta = channel->DimInfo[0].Delta;
    meta->DepLevel = delta;
    meta->EndDepth = channel->DimInfo[0].StartVal + delta * channel->DimInfo[0].Samples;
    for(int i = 0; i < 16; i++)
    {
        meta->Name[i] = entry->Name[i];
        meta->AliasName[i] = channel->AliasName[i];
        meta->AliasUnit[i] = channel->AliasUnit[i];
    }
    for(int i = 0; i < 8; i++)
    {
        meta->Unit[i] = channel->Unit[i];
        meta->DepthUnit[i] = channel->DimInfo[0].Unit[i];
    }
    meta->RepCode = channel->RepCode;
    meta->CodeLen = channel->CodeLen;
    meta->MinValue = channel->MinVal;
    meta->MaxValue = channel->MaxVal;

    return meta;
}

/**
 * @brief Get wave's meta
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return <ALP_WAVE> data
 */
void *AlpIO::waveMeta(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extract meta
    ALP_WAVE *meta = new ALP_WAVE;

    m_fileStream.seekg(entry->Position, std::ios_base::beg);

    ALP_CHANNEL *channel = new ALP_CHANNEL;
    m_fileStream.read(reinterpret_cast<char *>(channel), sizeof(ALP_CHANNEL));

    meta->DepthSamples = channel->DimInfo[0].Samples;

    meta->StartDepth = channel->DimInfo[0].StartVal;
    float delta = channel->DimInfo[0].Delta;
    meta->DepLevel = delta;
    meta->EndDepth = channel->DimInfo[0].StartVal + delta * channel->DimInfo[0].Samples;

    meta->TimeSamples = channel->DimInfo[1].Samples;
    meta->StartTime = channel->DimInfo[1].StartVal;
    float delta1 = channel->DimInfo[1].Delta;
    meta->TimeLevel = delta1;
    meta->EndTime = channel->DimInfo[1].StartVal + delta1 * channel->DimInfo[1].Samples;

    for(int i = 0; i < 16; i++)
    {
        meta->Name[i] = entry->Name[i];
        meta->AliasName[i] = channel->AliasName[i];
        meta->AliasUnit[i] = channel->AliasUnit[i];
    }
    for(int i = 0; i < 8; i++)
    {
        meta->Unit[i] = channel->Unit[i];
        meta->DepthUnit[i] = channel->DimInfo[0].Unit[i];
        meta->TimeUnit[i] = channel->DimInfo[1].Unit[i];
    }
    meta->RepCode = channel->RepCode;
    meta->CodeLen = channel->CodeLen;
    meta->MinValue = channel->MinVal;
    meta->MaxValue = channel->MaxVal;

    return meta;
}

/**
 * @brief Get FMT's meta
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return <ALP_FMT> data
 */
void *AlpIO::fmtMeta(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extract meta
    ALP_FMT *meta = new ALP_FMT;

    m_fileStream.seekg(entry->Position, std::ios_base::beg);

    ALP_CHANNEL *channel = new ALP_CHANNEL;
    m_fileStream.read(reinterpret_cast<char *>(channel), sizeof(ALP_CHANNEL));

    meta->TotalPointNum = channel->DimInfo[0].Samples;
    meta->MaxPointSamples = channel->DimInfo[1].MaxSamples;
    meta->StartTime = channel->DimInfo[1].StartVal;
    float delta1 = channel->DimInfo[1].Delta;
    meta->TimeLevel = delta1;

    for(int i = 0; i < 16; i++)
    {
        meta->Name[i] = entry->Name[i];
        meta->AliasName[i] = channel->AliasName[i];
        meta->AliasUnit[i] = channel->AliasUnit[i];
    }
    for(int i = 0; i < 8; i++)
    {
        meta->Unit[i] = channel->Unit[i];
        meta->DepthUnit[i] = channel->DimInfo[0].Unit[i];
        meta->TimeUnit[i] = channel->DimInfo[1].Unit[i];
    }
    meta->RepCode = channel->RepCode;
    meta->CodeLen = channel->CodeLen;
    meta->MinValue = channel->MinVal;
    meta->MaxValue = channel->MaxVal;

    return meta;
}

void *AlpIO::tdtMeta(ALP_OBJECT_ENTRY *entry)
{

}

/**
 * @brief Get Table's meta
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return <ALP_TABLE> data
 */
void *AlpIO::tableMeta(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extract meta
    ALP_TABLE *meta = new ALP_TABLE;

    m_fileStream.seekg(entry->Position, std::ios_base::beg);
    m_fileStream.read(reinterpret_cast<char *>(meta), sizeof(ALP_TABLE));

    return meta;

}

void *AlpIO::tableMeta(QString name)
{
    for(int num = 0; num < m_tableList->count(); num++)
    {
        if(m_tableList->at(num)->Name == name)
        {
            ALP_TABLE *tableInfo = new ALP_TABLE;

            m_fileStream.seekg(m_tableList->at(num)->Position , std::ios_base::beg);
            m_fileStream.read(reinterpret_cast<char *>(tableInfo), sizeof(ALP_TABLE));

            return tableInfo;
        }
    }

    return NULL;
}

/**
 * @brief Get Stream's meta
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return <ALP_STREAM> data
 */
void *AlpIO::streamMeta(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extract meta
    ALP_STREAM *meta = new ALP_STREAM;

    m_fileStream.seekg(entry->Position, std::ios_base::beg);
    m_fileStream.read(reinterpret_cast<char *>(meta), sizeof(ALP_TABLE));

    return meta;

}

//------------------------------ REGION: Data ------------------------------
/**
 * @brief Get curve data
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return Data stored in Data<T1, T2, T2>
 */
void *AlpIO::curveData(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extrace data
    ALP_CURVE *meta = static_cast<ALP_CURVE*>(curvesMeta(entry));

    if(!meta) {
        return NULL;
    }

    // Update read position
    //m_fileStream.seekg(entry->Position + 2 * sizeof(ALP_HEAD) + 464, std::ios_base::beg);
//    m_fileStream.seekg(entry->Position + sizeof, std::ios_base::beg);

    return data(meta, meta->RepCode, CurveObject);
}

/**
 * @brief Get wave data
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return Data stored in Data<T1, T2, T2>
 */
void *AlpIO::waveData(ALP_OBJECT_ENTRY *entry)
{

    if (!entry) {
        return NULL;
    }

    // Extrace data
    ALP_WAVE *meta = static_cast<ALP_WAVE*>(waveMeta(entry));

    if(!meta) {
        return NULL;
    }

    // Update read position
//    m_fileStream.seekg((entry->Position / (m_head->BlockLen))*(m_head->BlockLen) + (m_head->BlockLen), std::ios_base::beg);
//    m_fileStream.seekg(entry->Position, std::ios_base::beg);
    return data(meta, meta->RepCode, WaveObject);
}

/**
 * @brief Get FMT data
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return Data stored in Data<T1, T2, T2>
 */
void *AlpIO::fmtData(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extrace data
    ALP_FMT *meta = static_cast<ALP_FMT*>(fmtMeta(entry));

    if(!meta) {
        return NULL;
    }

    // Update read position
//    m_fileStream.seekg((entry->Position / (m_head->BlockLen))*(m_head->BlockLen) + (m_head->BlockLen), std::ios_base::beg);
//    m_fileStream.seekg(entry->Position, std::ios_base::beg);
//    float depth;
//    int timeNum;
//    float value;

//    m_fileStream.read(reinterpret_cast<char*>(&depth), sizeof(float));
//    m_fileStream.read(reinterpret_cast<char*>(&timeNum), sizeof(int));
//    m_fileStream.read(reinterpret_cast<char*>(&value), sizeof(float));

    return data(meta, meta->RepCode, FMTObject);
}

/**
 * @brief Get TDT data
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return Data stored in Data<T1, T2, T2>
 */
void *AlpIO::tdtData(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extrace data
//    ALP_FMT *meta = static_cast<ALP_FMT*>(tdtMeta(entry));

//    if(!meta) {
//        return NULL;
//    }

    return NULL;
}

/**
 * @brief Get Table data
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return Data stored in Data<T1, T2, T2>
 */
void *AlpIO::tableData(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extrace data
    ALP_TABLE *meta = static_cast<ALP_TABLE*>(tableMeta(entry));

    if(!meta) {
        return NULL;
    }

    // Table data
    for(int i = 0; i < meta->FieldCount; i++)
    {
        ALP_TABLE_FIELD *alpTableField = new ALP_TABLE_FIELD;
        m_fileStream.read(reinterpret_cast<char*>(alpTableField), sizeof(ALP_TABLE_FIELD));

        m_alpTableField->append(alpTableField);
    }

    QList<Table*> *recordList = new QList<Table*>();
    for(int i = 0; i < meta->RecordCount; i++)
    {
        Table *record = new Table();

        for(int j = 0; j < m_alpTableField->count(); j++)
        {
            ALP_TABLE_FIELD *field = m_alpTableField->at(j);

            if(field->RepCode == UnsignedCharData) {
                char *data = new char[field->Length];
                m_fileStream.read(reinterpret_cast<char*>(data), field->Length*sizeof(char));
                record->setProperty(field->Name, QString(data));
            }
            if(field->RepCode == LongData
                    || field->RepCode == UnsignedLongData
                    || field->RepCode == ShortData
                    || field->RepCode == UnsignedShortData
                    || field->RepCode == CharData) {
                int *data = new int;
                m_fileStream.read(reinterpret_cast<char*>(data), sizeof(int));
                record->setProperty(field->Name,*data);
            }
            if(field->RepCode == FloatData || field->RepCode == DoubleData) {
                float *data = new float;
                m_fileStream.read(reinterpret_cast<char*>(data), sizeof(float));
                record->setProperty(field->Name,*data);
            }
        }

        recordList->append(record);
    }

    return recordList;
}

void *AlpIO::tableData(QString name)
{
    ALP_OBJECT_ENTRY *objectEntry = new ALP_OBJECT_ENTRY;
    ALP_TABLE *tableInfo = new ALP_TABLE;

    for(int num = 0; num < m_tableList->count(); num++)
    {
        if(m_tableList->at(num)->Name == name)
        {
            objectEntry = m_tableList->at(num);
            tableInfo = static_cast<ALP_TABLE*>(tableMeta(name));
        }
    }

    if(objectEntry == NULL || tableInfo == NULL)
        return NULL;

    for(int i = 0; i < tableInfo->FieldCount; i++)
    {
        ALP_TABLE_FIELD *alpTableField = new ALP_TABLE_FIELD;
        m_fileStream.read(reinterpret_cast<char*>(alpTableField), sizeof(ALP_TABLE_FIELD));

        m_alpTableField->append(alpTableField);
    }

    QList<Table*> *recordList = new QList<Table*>();
    for(int i = 0; i < tableInfo->RecordCount; i++)
    {
        Table *record = new Table();

        for(int j = 0; j < m_alpTableField->count(); j++)
        {
            ALP_TABLE_FIELD *field = m_alpTableField->at(j);

            if(field->RepCode == UnsignedCharData) {
                char *data = new char[field->Length];
                m_fileStream.read(reinterpret_cast<char*>(data), field->Length*sizeof(char));
                record->setProperty(field->Name, QString(data));
            }
            if(field->RepCode == LongData
                    || field->RepCode == UnsignedLongData
                    || field->RepCode == ShortData
                    || field->RepCode == UnsignedShortData
                    || field->RepCode == CharData) {
                int *data = new int;
                m_fileStream.read(reinterpret_cast<char*>(data), sizeof(int));
                record->setProperty(field->Name,*data);
            }
            if(field->RepCode == FloatData || field->RepCode == DoubleData) {
                float *data = new float;
                m_fileStream.read(reinterpret_cast<char*>(data), sizeof(float));
                record->setProperty(field->Name,*data);
            }
        }

        recordList->append(record);
    }

    return recordList;
}

/**
 * @brief Get Stream data
 * @param entry Object entry <ALP_OBJECT_ENTRY>
 * @return Data stored in Data<T1, T2, T2>
 */
void *AlpIO::streamData(ALP_OBJECT_ENTRY *entry)
{
    if (!entry) {
        return NULL;
    }

    // Extrace data
    ALP_STREAM *meta = static_cast<ALP_STREAM*>(streamMeta(entry));

    if(!meta) {
        return NULL;
    }

    // Stream data
//    QList<char> *dataList = new QList<char>();

//    for(int i = 0; i < meta->Length; i++)
//    {
//        char data;

//        m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(char));

//        dataList->append(data);
//    }
    char *tempData = new char[meta->Length];
    m_fileStream.read(tempData, meta->Length);

    return tempData;

//    return dataList;
}


void *AlpIO::constData(QString name)
{
    QStringList tableNameList = name.split(',');

    Table *data = new Table();

    for(int num = 0; num < tableNameList.count(); num++)
    {
        QString tableName = tableNameList.at(num);

        QList<Table*> *recordList = static_cast<QList<Table*>*>(tableData(tableName));
        for(int i = 0; i < recordList->count(); i++)
        {
            Table *record = recordList->at(i);

            QString name = record->property(m_alpTableField->at(0)->Name).toString();
            QString value = record->property(m_alpTableField->at(1)->Name).toString();

            data->setProperty(name.toStdString().c_str(), value);
        }

        m_alpTableField->clear();
    }

    return data;
}

void AlpIO::updateHead()
{
    int pos = m_fileWriteStream.tellp();
    m_fileWriteStream.seekp(10, std::ios_base::beg);
    m_fileWriteStream.write((char *)m_head, sizeof(ALP_HEAD));
    m_fileWriteStream.seekp(pos, std::ios_base::beg);
}

void AlpIO::updateEntry(ALP_OBJECT_ENTRY *entry)
{
    int pos = m_fileWriteStream.tellp();
    m_fileWriteStream.seekp(entry->Position - sizeof(ALP_OBJECT_ENTRY), std::ios_base::beg);
    m_fileWriteStream.write((char *)entry, sizeof(ALP_OBJECT_ENTRY));
    m_fileWriteStream.seekp(pos, std::ios_base::beg);
}

//------------------------------ REGION: Read Data ------------------------------
void *AlpIO::data(void *meta, int datatype, int objectType)
{
    switch (datatype) {
    case CharData:
        return dataChar(meta, objectType);
    case ShortData:
        return dataShort(meta, objectType);
    case LongData:
        return dataLong(meta, objectType);
    case FloatData:
        return dataFloat(meta, objectType);
    case DoubleData:
        return dataDouble(meta, objectType);
    case UnsignedCharData:
        return dataUnsignedChar(meta, objectType);
    case UnsignedShortData:
        return dataUnsignedShort(meta, objectType);
    case UnsignedLongData:
        return dataUnsignedLong(meta, objectType);
    default:
        return NULL;
    }
}

void *AlpIO::dataChar(void *info, int type)
{
    Data<float,float,char> *dataChar = new Data<float,float,char>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            QList<QPair<float,char>*> *timeAndValue = new QList<QPair<float,char>*>();
            QPair<float,char> *timePair = new QPair<float,char>();
            char data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(char));
            timePair->first = time;
            timePair->second = data;
            timeAndValue->append(timePair);

            dataChar->data.insert(depth,timeAndValue);
        }

        return dataChar;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            QList<QPair<float,char>*> *timeAndValue = new QList<QPair<float,char>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,char> *timePair = new QPair<float,char>();
                char data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(char));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataChar->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }

        return dataChar;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            float timeSamples;
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,char>*> *timeAndValue = new QList<QPair<float,char>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,char> *timePair = new QPair<float,char>();
                char data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(char));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataChar->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }

        return dataChar;
    }
    default:
        return NULL;
    }
}

void *AlpIO::dataShort(void *info, int type)
{
    Data<float,float,short> *dataShort = new Data<float,float,short>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            QList<QPair<float,short>*> *timeAndValue = new QList<QPair<float,short>*>();
            QPair<float,short> *timePair = new QPair<float,short>();
            short data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(short));
            timePair->first = time;
            timePair->second = data;;
            timeAndValue->append(timePair);

            dataShort->data.insert(depth,timeAndValue);
        }
        return dataShort;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            QList<QPair<float,short>*> *timeAndValue = new QList<QPair<float,short>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,short> *timePair = new QPair<float,short>();
                short data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(short));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataShort->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }
        return dataShort;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            float timeSamples;

            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,short>*> *timeAndValue = new QList<QPair<float,short>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,short> *timePair = new QPair<float,short>();
                short data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(short));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataShort->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }
        return dataShort;
    }
    default:
        return NULL;
    }
}

void *AlpIO::dataLong(void *info, int type)
{
    Data<float,float,long> *dataLong = new Data<float,float,long>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            QList<QPair<float,long>*> *timeAndValue = new QList<QPair<float,long>*>();
            QPair<float,long> *timePair = new QPair<float,long>();
            long data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(long));
            timePair->first = time;
            timePair->second = data;
            timeAndValue->append(timePair);

            dataLong->data.insert(depth,timeAndValue);
        }

        return dataLong;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            QList<QPair<float,long>*> *timeAndValue = new QList<QPair<float,long>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,long> *timePair = new QPair<float,long>();
                long data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(long));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataLong->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }

        return dataLong;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            float timeSamples;

            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,long>*> *timeAndValue = new QList<QPair<float,long>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,long> *timePair = new QPair<float,long>();
                long data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(long));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataLong->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }

        return dataLong;
    }
    default:
        return NULL;
    }
}

void *AlpIO::dataFloat(void *info, int type)
{
    Data<float,float,float> *dataFloat = new Data<float,float,float>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            // Depth
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            // Time and value
            QList<QPair<float,float>*> *timeAndValue = new QList<QPair<float,float>*>();
            QPair<float,float> *timePair = new QPair<float,float>();
            float data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(float));
            timePair->first = time;
            timePair->second = data;
            timeAndValue->append(timePair);

            // New data item
            dataFloat->data.insert(depth, timeAndValue);
        }

        return dataFloat;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            // Depth
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            // Time and value
            QList<QPair<float,float>*> *timeAndValue = new QList<QPair<float,float>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {                
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,float> *timePair = new QPair<float,float>();
                float data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(float));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataFloat->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }

        return dataFloat;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            float timeSamples;

            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,float>*> *timeAndValue = new QList<QPair<float,float>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,float> *timePair = new QPair<float,float>();
                float data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(float));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataFloat->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }

        return dataFloat;
    }
    case TDTObject:
        return NULL;
    default:
        return NULL;
    }
}

void *AlpIO::dataDouble(void *info, int type)
{
    Data<float,float,double> *dataDouble = new Data<float,float,double>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            QList<QPair<float,double>*> *timeAndValue = new QList<QPair<float,double>*>();
            QPair<float,double> *timePair = new QPair<float,double>();
            double data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(double));
            timePair->first = time;
            timePair->second = data;
            timeAndValue->append(timePair);
            dataDouble->data.insert(depth,timeAndValue);
        }

        return dataDouble;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            QList<QPair<float,double>*> *timeAndValue = new QList<QPair<float,double>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,double> *timePair = new QPair<float,double>();
                double data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(double));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataDouble->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }

        return dataDouble;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            float timeSamples;

            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,double>*> *timeAndValue = new QList<QPair<float,double>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,double> *timePair = new QPair<float,double>();
                double data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(double));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataDouble->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }
        return dataDouble;
    }
    default:
        return NULL;
    }
}

void *AlpIO::dataUnsignedChar(void *info, int type)
{
    Data<float,float,unsigned char> *dataUChar = new Data<float,float,unsigned char>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            QList<QPair<float,unsigned char>*> *timeAndValue = new QList<QPair<float,unsigned char>*>();
            QPair<float,unsigned char> *timePair = new QPair<float,unsigned char>();
            unsigned char data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned char));
            timePair->first = time;
            timePair->second = data;
            timeAndValue->append(timePair);
            dataUChar->data.insert(depth,timeAndValue);
        }

        return dataUChar;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            QList<QPair<float,unsigned char>*> *timeAndValue = new QList<QPair<float,unsigned char>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,unsigned char> *timePair = new QPair<float,unsigned char>();
                unsigned char data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned char));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataUChar->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }

        return dataUChar;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            float timeSamples;

            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,unsigned char>*> *timeAndValue = new QList<QPair<float,unsigned char>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,unsigned char> *timePair = new QPair<float,unsigned char>();
                unsigned char data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned char));                
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataUChar->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }

        return dataUChar;
    }
    default:
        return NULL;
    }
}

void *AlpIO::dataUnsignedShort(void *info, int type)
{
    Data<float,float,unsigned short> *dataUShort = new Data<float,float,unsigned short>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            QList<QPair<float,unsigned short>*> *timeAndValue = new QList<QPair<float,unsigned short>*>();
            QPair<float,unsigned short> *timePair = new QPair<float,unsigned short>();
            unsigned short data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned short));
            timePair->first = time;
            timePair->second = data;
            timeAndValue->append(timePair);
            dataUShort->data.insert(depth,timeAndValue);
        }

        return dataUShort;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            QList<QPair<float,unsigned short>*> *timeAndValue = new QList<QPair<float,unsigned short>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,unsigned short> *timePair = new QPair<float,unsigned short>();
                unsigned short data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned short));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataUShort->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }

        return dataUShort;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            float timeSamples;

            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,unsigned short>*> *timeAndValue = new QList<QPair<float,unsigned short>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,unsigned short> *timePair = new QPair<float,unsigned short>();
                unsigned short data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned short));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataUShort->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }

        return dataUShort;
    }
    default:
        return NULL;
    }
}

void *AlpIO::dataUnsignedLong(void *info, int type)
{
    Data<float,float,unsigned long> *dataULong = new Data<float,float,unsigned long>();
    ALP_CURVE *curveInfo = new ALP_CURVE;
    ALP_WAVE *waveInfo = new ALP_WAVE;
    ALP_FMT *fmtInfo = new ALP_FMT;
    float depth,time;

    switch (type) {
    case CurveObject: {
        curveInfo = static_cast<ALP_CURVE*>(info);
        depth = curveInfo->StartDepth;
        time = -1;

        for(int i = 0; i < curveInfo->DepthNum; i++)
        {
            depth = curveInfo->StartDepth + i * curveInfo->DepLevel;

            QList<QPair<float,unsigned long>*> *timeAndValue = new QList<QPair<float,unsigned long>*>();
            QPair<float,unsigned long> *timePair = new QPair<float,unsigned long>();
            unsigned long data;

            m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned long));
            timePair->first = time;
            timePair->second = data;
            timeAndValue->append(timePair);
            dataULong->data.insert(depth,timeAndValue);
        }

        return dataULong;
    }
    case WaveObject: {
        waveInfo = static_cast<ALP_WAVE*>(info);
        depth = waveInfo->StartDepth;
        time = waveInfo->StartTime;

        for(int i = 0; i < waveInfo->DepthSamples; i++)
        {
            depth = waveInfo->StartDepth + i * waveInfo->DepLevel;

            QList<QPair<float,unsigned long>*> *timeAndValue = new QList<QPair<float,unsigned long>*>();
            for(int j = 0; j < waveInfo->TimeSamples; j++)
            {
                time = waveInfo->StartTime + j * waveInfo->TimeLevel;

                QPair<float,unsigned long> *timePair = new QPair<float,unsigned long>();
                unsigned long data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned long));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataULong->data.insert(depth,timeAndValue);
            time = waveInfo->StartTime;
        }

        return dataULong;
    }
    case FMTObject: {
        fmtInfo = static_cast<ALP_FMT*>(info);
        time = fmtInfo->StartTime;

        for(int i = 0; i < fmtInfo->TotalPointNum; i++)
        {
            float timeSamples;

            m_fileStream.read(reinterpret_cast<char *>(&depth), sizeof(float));
            m_fileStream.read(reinterpret_cast<char *>(&timeSamples), sizeof(float));

            QList<QPair<float,unsigned long>*> *timeAndValue = new QList<QPair<float,unsigned long>*>();
            for(int j = 0; j < timeSamples; j++)
            {
                time = fmtInfo->StartTime + j * fmtInfo->TimeLevel;

                QPair<float,unsigned long> *timePair = new QPair<float,unsigned long>();
                unsigned long data;

                m_fileStream.read(reinterpret_cast<char *>(&data), sizeof(unsigned long));
                timePair->first = time;
                timePair->second = data;
                timeAndValue->append(timePair);
            }

            dataULong->data.insert(depth,timeAndValue);
            time = fmtInfo->StartTime;
        }

        return dataULong;
    }
    default:
        return NULL;
    }
}
//=======================================new end============================================
