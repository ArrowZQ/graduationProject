#-------------------------------------------------
#
# Project created by QtCreator 2014-05-02T15:53:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AlpFileDemo
TEMPLATE = app


SOURCES += main.cpp\
    spreadsheet.cpp \
    mainwindow.cpp \
    listObject.cpp \

HEADERS  += \
    spreadsheet.h \
    mainwindow.h \
    listObject.h \

FORMS    += mainwindow.ui

LIBS += \
        -L"$${PWD}/../data/lib" -lAlpData

INCLUDEPATH += \
        $${PWD}/../data
