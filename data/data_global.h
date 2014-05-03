/**************************************************************************************************
**
** @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
**
** Module: ALP Application
** File: data_global.h
** Description: .
**
** Created By: micle, micle@xdt-inc.com
** Created Date: 2014-1-15T10:00:00
**
***************************************************************************************************/

#ifndef DATA_GLOBAL_H
#define DATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DATA_LIBRARY)
#  define DATASHARED_EXPORT Q_DECL_EXPORT
#else
#  define DATASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DATA_GLOBAL_H
