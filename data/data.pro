###################################################################################################
##
## @2013-2014, Xi'an DATA Technology for Electronic Information Inc.
##
## File: data.pro
## Description: Data precossing and data file management.
##
## Created By: Micle Bu, micle@xdt-inc.com
## Created Date: 2014-01-15T10:11:02
##
###################################################################################################

QT       += core gui widgets xml xmlpatterns

TARGET = AlpData
TEMPLATE = lib

include(data_config.pri)
#include($${ALP_ROOT}/common.pri)

DESTDIR = $${ALP_ROOT}/lib

DEFINES += DATA_LIBRARY

SOURCES += \
    io.cpp \
    fileio.cpp \
    table.cpp \
    metadata.cpp \
    alpio.cpp \
    fileshare.cpp \
    filereader.cpp\

HEADERS +=\
    data_global.h \
    io.h \
    fileio.h \
    alpio.h \
    fileshare.h \
    filereader.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
