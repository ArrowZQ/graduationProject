#include "spreadsheet.h"
using namespace ALP;

Sheet::Sheet(QWidget *parent) :
    QTableWidget(parent)
{
//    QHBoxLayout *hlayout = new QHBoxLayout();
    //tableWidget = new QTableWidget(50,50, this);
//    tableWidget->showMaximized();

//    QListWidget *listWidget = new QListWidget(parent);
//    QListWidgetItem *newItem = new QListWidgetItem;
//    newItem->setText("itemText");
//     listWidget->insertItem(1, newItem);
//    hlayout->addWidget(listWidget);
//    hlayout->addWidget(tableWidget);

//    listWidget->show();
    //    tableWidget->show();
}

void Sheet::openfile()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QDir::homePath(),
                                                    tr("ALP (*.ALP)"));
//    AlpIO *alp = new AlpIO();
//    alp->openFile(fileName);
    m_listEntry->clear();
    FileShare::removeFile(fileName);
    FileReader reader;
    if (!reader.open(fileName)) {
        qDebug()<<"open error";
        return;
    }
    QVector<DataListItem> list = reader.list();
    for (int i = 0; i < list.count(); i++) {
        QListWidgetItem *listitem = new QListWidgetItem();
        listitem->setText(QString(list.at(i).Name));
        listitem->setData(Qt::ToolTipRole,list.at(i).Type);
        listitem->setData(Qt::StatusTipRole, list.at(i).Seq);
        m_listEntry->addItem(listitem);
    }
    m_fileName = fileName;
}

void Sheet::createSample()
{
    m_listEntry->clear();
    FileWrite *write = new FileWrite();
    QString fileName = write->createNewFile("sample", QDir::homePath(), FileIO::ALPFile);
    if (fileName.isEmpty()) {
        return;
    }
    FileShare::removeFile(fileName);
    write->open(fileName);
//    AlpIO *alp = new AlpIO();
//    alp->createNewAlpFile(QDir::homePath() + "/sample.ALP");
//    alp->openFile(QDir::homePath() + "/sample.ALP");

//    ALP_OBJECT_ENTRY *entry = new ALP_OBJECT_ENTRY;
//    strcpy(entry->Name,"Time");
//    entry->Status = FileIO::normal;
//    entry->Attribute = FileIO::StreamAttribure;
//    entry->SubAttribute = FileIO::image;

//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
//                                                    QDir::homePath(),
//                                                tr("Image Files (*.png *.jpg *.bmp)"));
//    QByteArray ba;
//    QPixmap pixmap(fileName);
//    QDataStream ds(&ba, QIODevice::WriteOnly);
//    ds << pixmap;
//    QByteArray bas("Hello world");
//    char * temp = bas.data();
//    while (*temp) {
//        qDebug() << "[" << *temp << "]";
//        ++temp;
//    }
//    qDebug()<<alp->writeStream(entry,ba.data());
    qDebug()<<"::"<<sizeof(ALP_HEAD)<<" " <<sizeof(ALP_OBJECT_ENTRY);
    ALP_OBJECT_ENTRY *entry2 = AlpData::entry("Ten", AlpData::normal,AlpData::ChannelAttribure,
                                              AlpData::CurveObject);
    ALP_CHANNEL_DIMENSION *dimone = AlpData::channelDim("depth","m","D",2,AlpData::FloatData);
    ALP_CHANNEL *channel = AlpData::channel("nono","ten","nonos",AlpData::FloatData,dimone);
//    ALP_OBJECT_ENTRY *entry2 = new ALP_OBJECT_ENTRY;
//    strcpy(entry2->Name,"Ten");
//    entry2->Status = AlpData::normal;
//    entry2->Attribute = AlpData::ChannelAttribure;
//    entry2->SubAttribute = AlpData::CurveObject;
//    ALP_CHANNEL *channel = new ALP_CHANNEL;
//    strcpy(channel->Unit, "nono");
//    strcpy(channel->AliasName,"ten");
//    strcpy(channel->AliasUnit,"nonos");
//    channel->RepCode = AlpData::FloatData;
//    channel->CodeLen = 4;
//    channel->MinVal = 0;
//    channel->MaxVal = 200;
//    channel->NumOfDimension = 1;

//    strcpy(channel->DimInfo[0].Name,"depht");
//    strcpy(channel->DimInfo[0].Unit, "m");
//    strcpy(channel->DimInfo[0].AliasName,"D");
//    channel->DimInfo[0].StartVal = 1000;
//    channel->DimInfo[0].Delta = 2;
//    channel->DimInfo[0].Samples = 500;
//    channel->DimInfo[0].MaxSamples = 1000;
//    channel->DimInfo[0].Size = 4;
//    channel->DimInfo[0].RepCode = AlpData::FloatData;

    Data<float,float,float> *data = new Data<float,float,float>();
    QMap<float,QList<QPair<float,float>*>*> dd = data->data;
    for (int p = 0; p < channel->DimInfo[0].Samples; p++) {
        QList<QPair< float, float> *> *list = new QList<QPair< float, float> *>();
        QPair<float, float> *pair = new QPair<float, float>();
        pair->first = -1;
        pair->second = qrand();
        list->append(pair);
        data->data.insert(p *channel->DimInfo[0].Delta + channel->DimInfo[0].StartVal, list);
    }



    /************************多维*************************************/
    ALP_OBJECT_ENTRY  *entry3 = new ALP_OBJECT_ENTRY;
    strcpy(entry3->Name,"dImg");
    entry3->Status = AlpData::normal;
    entry3->Attribute = AlpData::ChannelAttribure;
    entry3->SubAttribute = AlpData::WaveObject;
    ALP_CHANNEL *channel3 = new ALP_CHANNEL;
    strcpy(channel3->Unit, "NO");
    strcpy(channel3->AliasName,"dimg");
    strcpy(channel3->AliasUnit,"no");
    channel3->RepCode = AlpData::FloatData;
    channel3->CodeLen = 4;
    channel3->MinVal = 0;
    channel3->MaxVal = 200;
    channel3->NumOfDimension = 2;

    strcpy(channel3->DimInfo[0].Name,"depht");
    strcpy(channel3->DimInfo[0].Unit, "m");
    strcpy(channel3->DimInfo[0].AliasName,"D");
    channel3->DimInfo[0].StartVal = 1000;
    channel3->DimInfo[0].Delta = 1;
    channel3->DimInfo[0].Samples = 600;
    channel3->DimInfo[0].MaxSamples = 1000;
    channel3->DimInfo[0].Size = 4;
    channel3->DimInfo[0].RepCode = AlpData::FloatData;

    strcpy(channel3->DimInfo[1].Name,"time");
    strcpy(channel3->DimInfo[1].Unit, "us");
    strcpy(channel3->DimInfo[1].AliasName,"t");
    channel3->DimInfo[1].StartVal = 0;
    channel3->DimInfo[1].Delta = 2;
    channel3->DimInfo[1].Samples = 100;
    channel3->DimInfo[1].MaxSamples = 1000;
    channel3->DimInfo[1].Size = 4;
    channel3->DimInfo[1].RepCode = AlpData::FloatData;

    Data<float,float,float> *data3 = new Data<float,float,float>();
    for (int p = 0; p < channel3->DimInfo[0].Samples; p++) {
        QList<QPair< float, float> *> *list = new QList<QPair< float, float> *>();
        for (int j = 0; j < channel3->DimInfo[1].Samples; j ++){
            QPair<float, float> *pair = new QPair<float, float>;
            pair->first = j * channel3->DimInfo[1].Delta + channel3->DimInfo[1].StartVal;
            pair->second = j;
            list->append(pair);
        }
        data3->data.insert(p * channel3->DimInfo[0].Delta + channel3->DimInfo[0].StartVal, list);
    }
//    qDebug()<<alp->writeChannelData(entry2,channel,data);
//    qDebug()<<alp->writeChannelData(entry3,channel3,data3);
    qDebug()<<write->writeChannelData(entry3,channel3,data3);
    qDebug()<<write->writeChannelData(entry2,channel,data);
}

void Sheet::doubleClicked(QListWidgetItem *itemClick)
{
    FileShare::removeFile(m_fileName);
    FileReader reader;
    if (!reader.open(m_fileName)) {
        qDebug()<<"open error";
        return;
    }
    if (itemClick->data(Qt::ToolTipRole).toInt() == AlpData::CurveObject) {
        QPolygonF poly = reader.curveData(itemClick->text());
        setColumnCount(1);
        setRowCount(poly.size());

        QStringList horizontalHeaderLabel;
        QStringList verticalheaderLabel;
        for(int i = 0; i < poly.size(); i++){
            QTableWidgetItem *twi = new QTableWidgetItem();
            twi->setText(QString::number(poly.at(i).x()));
            setItem(i,0,twi);
            verticalheaderLabel << QString::number(poly.at(i).y());
        }
        horizontalHeaderLabel << "x";
        setHorizontalHeaderLabels(horizontalHeaderLabel);
        setVerticalHeaderLabels(verticalheaderLabel);
    } else if (itemClick->data(Qt::ToolTipRole).toInt() == AlpData::WaveObject) {
        Data<float,float,float> *waveData = static_cast<Data<float,float,float> *>(reader.data(itemClick->text()));
        MetaData *meta = reader.meta(itemClick->text());
        int raw = meta->property("depthSample").toInt();
        int col = meta->property("timeSample").toInt();
        setColumnCount(col);
        setRowCount(raw);

        QStringList horizontalHeaderLabel;
        QStringList verticalheaderLabel;
        QMap<float,QList<QPair<float,float>*>*>::const_iterator i = waveData->data.constBegin();
        int whichRow = 0;
        while (i != waveData->data.constEnd()) {
            QList<QPair<float,float>*>* tempval = i.value();
            for (int k = 0; k < tempval->count(); k++) {
                QTableWidgetItem *twi = new QTableWidgetItem();
                twi->setText(QString::number(tempval->at(k)->second));
                setItem(whichRow,k,twi);
                if (whichRow == 0) {
                    horizontalHeaderLabel << QString::number( tempval->at(k)->first);
                }
            }

            verticalheaderLabel << QString::number( i.key());
            whichRow++;
            i++;
        }
        setHorizontalHeaderLabels(horizontalHeaderLabel);
        setVerticalHeaderLabels(verticalheaderLabel);
    }
}

void Sheet::rightClicked(QListWidgetItem *item)
{
    bool ok;
        QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("User name:"), QLineEdit::Normal,
                                             item->text(), &ok);
        if (ok && !text.isEmpty()){
            int seq  = item->data(Qt::StatusTipRole).toInt();
            FileShare::removeFile(m_fileName);
            FileWrite write;
            if (!write.open(m_fileName)) {
                qDebug()<<"open error";
                return;
            }
            if (write.updateName(seq, text)) {
                item->setText(text);
            }
        }
}
QListWidget *Sheet::listEntry() const
{
    return m_listEntry;
}

void Sheet::setListEntry(QListWidget *listEntry)
{
    m_listEntry = listEntry;
}

void Sheet::test()
{

}

