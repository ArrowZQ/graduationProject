#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <QDebug>
#include <QTableWidget>
#include <QListWidget>
#include <QLayout>
#include <QMainWindow>
#include <QFileDialog>
#include <QObject>
#include <QPolygonF>

#include "filereader.h"

using namespace ALP;

class Sheet : public QTableWidget
{
    Q_OBJECT

public:
    Sheet(QWidget *parent = 0);

    QListWidget *listEntry() const;
    void setListEntry(QListWidget *listEntry);

public slots:
    void openfile();
    void createSample();
    void doubleClicked(QListWidgetItem * item);


private:
    QTableWidget *tableWidget;
    QListWidget *m_listEntry;
    QString m_fileName;
};
#endif // SPREADSHEET_H
