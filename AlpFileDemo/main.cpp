#include "mainwindow.h"
#include <QApplication>
#include "spreadsheet.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.showMaximized();
//    Sheet *sheet = new Sheet();
    return a.exec();
}
