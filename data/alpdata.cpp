/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: alpdata.cpp
** Description: AlpData.
**
** Created By: tang, tang@xdt-inc.com
** Created Date: 2013-10-12T20:42:35
**
***************************************************************************************************/

#include "alpdata.h"

AlpData::AlpData()
{
}

ALP_OBJECT_ENTRY *AlpData::entry(QString name, ObjectStatus status, ObjectAttribute attr, ObjectType subattr)
{
    ALP_OBJECT_ENTRY *entry = new ALP_OBJECT_ENTRY;
    strcpy(entry->Name, name.toLocal8Bit().data());
    entry->Status = status;
    entry->Attribute = attr;
    entry->SubAttribute = subattr;

    return entry;
}

ALP_CHANNEL *AlpData::channel(QString unit, QString aName, QString aUnit, AlpData::DataType datatype, ALP_CHANNEL_DIMENSION *oneDim, ALP_CHANNEL_DIMENSION *twoDim, ALP_CHANNEL_DIMENSION *threeDim, ALP_CHANNEL_DIMENSION *fourDim)
{
    ALP_CHANNEL *channel = new ALP_CHANNEL;
    strcpy(channel->Unit, unit.toLocal8Bit().data());
    strcpy(channel->AliasName, aName.toLocal8Bit().data());
    strcpy(channel->AliasUnit, aUnit.toLocal8Bit().data());
    channel->RepCode = datatype;
    channel->CodeLen = dataTypeLen(datatype);
    channel->NumOfDimension = 0;
    if (oneDim == NULL) {
        delete channel;
        return NULL;
    }
    channel->DimInfo[0] = *oneDim;
    channel->NumOfDimension++;

    if (twoDim != NULL) {
        channel->DimInfo[1] = *twoDim;
        channel->NumOfDimension++;
    }
    if (threeDim != NULL) {
        channel->DimInfo[2] = *threeDim;
        channel->NumOfDimension++;
    }
    if (fourDim != NULL) {
        channel->DimInfo[1] = *fourDim;
        channel->NumOfDimension++;
    }

    return channel;

}

ALP_CHANNEL_DIMENSION *AlpData::channelDim(QString name, QString unit, QString aName, float delta, AlpData::DataType datatype)
{
    ALP_CHANNEL_DIMENSION *dim = new ALP_CHANNEL_DIMENSION;
    strcpy(dim->Name, name.toLocal8Bit().data());
    strcpy(dim->Unit, unit.toLocal8Bit().data());
    strcpy(dim->AliasName, aName.toLocal8Bit().data());
    dim->Delta = delta;
    dim->RepCode = datatype;
    dim->Size = dataTypeLen(datatype);
    dim->Samples = 500;
    dim->StartVal = 100;

    return dim;
}

int AlpData::dataTypeLen(AlpData::DataType datatype)
{
    if (datatype == CharData) {
        return sizeof(char);
    } else if (datatype == ShortData) {
        return sizeof(short);
    } else if (datatype == LongData) {
        return sizeof(long);
    } else if (datatype == FloatData) {
        return sizeof(float);
    } else if (datatype == DoubleData) {
        return sizeof(double);
    } else if (datatype == UnsignedCharData) {
        return sizeof(unsigned char);
    } else if (datatype == UnsignedShortData) {
        return sizeof(unsigned short);
    } else if (datatype == UnsignedLongData) {
        return sizeof(unsigned long);
    } else {
        return -1;
    }
}
