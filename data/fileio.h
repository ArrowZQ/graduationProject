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
#include "alpdata.h"

namespace ALP {

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
     * @brief DataType
     */

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
    virtual bool updateAliasName(int seq, QString aName);
    virtual bool deleteObject(int seq);
    virtual bool dicardObject(int seq);
    virtual bool recoverObject(int seq);
    virtual bool writeChannelData(ALP_OBJECT_ENTRY *entry ,ALP_CHANNEL *channel, Data<float,float,float> *data);
    virtual bool writeTableData(ALP_OBJECT_ENTRY *entry, QList<ALP_TABLE_FIELD *> listTableField, QList<QPair<int, QList<void *> > > recoders);
    virtual bool writeStreamData(ALP_OBJECT_ENTRY *entry, const char *buf, int len);


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
