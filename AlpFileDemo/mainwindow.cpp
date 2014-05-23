#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QWidget *w = new QWidget();
    QVBoxLayout *mainlayout = new QVBoxLayout();
    QHBoxLayout *hlayoutBut = new QHBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout();
    Sheet *sheet = new Sheet(this);

    QListWidget *listWidget = new QListWidget(parent);
    sheet->setListEntry(listWidget);
//    QListWidgetItem *newItem = new QListWidgetItem;
//    newItem->setText("itemText");
//    listWidget->insertItem(1, newItem);
    listWidget->setFixedWidth(200);

    QPushButton *openfile = new QPushButton();
    openfile->setText("open file");
    openfile->setFixedWidth(100);

    QPushButton *createSample = new QPushButton();
    createSample->setText("create sample");
    createSample->setFixedWidth(100);

    connect(openfile, SIGNAL(clicked()), sheet, SLOT(openfile()));
    connect(createSample, SIGNAL(clicked()), sheet, SLOT(createSample()));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), sheet, SLOT(doubleClicked(QListWidgetItem*)));
    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), sheet, SLOT(rightClicked(QListWidgetItem*)));

    hlayoutBut->addWidget(openfile,0,Qt::AlignLeft);
    hlayoutBut->addWidget(createSample,1,Qt::AlignLeft);

    hlayout->addWidget(listWidget);
    hlayout->addWidget(sheet);
    //hlayout->setStretch();

    mainlayout->addLayout(hlayoutBut);
    mainlayout->addLayout(hlayout);

    w->setLayout(mainlayout);

    setCentralWidget(w);

}

MainWindow::~MainWindow()
{
}
