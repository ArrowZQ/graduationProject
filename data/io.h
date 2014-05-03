/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Data Lib
** File: io.h
** Description: IO.
**
** Created By: wangshuo, wangshuo@xdt-inc.com
** Created Date: 2014-1-15
**
***************************************************************************************************/

#ifndef IO_H
#define IO_H

#include "data_global.h"
#include <QString>
#include <iostream>

namespace ALP {

/**
 * @brief The IO class
 *
 * @author wangshuo
 * @author Micle Bu
 * @date 2014-1-15
 */
class DATASHARED_EXPORT IO
{
public:
    virtual bool initialization() = 0;
    virtual void *list(int type = 0) = 0;
    virtual void *meta(int seq) = 0;
    virtual void *data(int seq) = 0;
};

}

#endif // IO_H
