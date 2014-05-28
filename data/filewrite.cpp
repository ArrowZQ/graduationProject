#include "filewrite.h"

FileWrite::FileWrite() : m_fileIO(NULL)
{
}

bool FileWrite::open(const QString &fileName)
{
    // File checking
    if (!QFile::exists(fileName)) {
        m_isOpen = false;

        return m_isOpen;
    }

    m_fileName = fileName;
    m_fileType = fileType(fileName);
    m_fileIO = FileShare::file(fileName);

    if (!m_fileIO) {
        switch (m_fileType) {
        case FileIO::ALPFile:
        {
            AlpIO *alp = new AlpIO();
            alp->openFile(fileName);
            m_fileIO = alp;
            m_isOpen = m_fileIO->isOpen();
            break;
        }
        case FileIO::AXPFile:
            break;
        default:
            return false;
        }

        // Add file to fileshare
        if (!FileShare::addFile(fileName, m_fileIO)) {
            return false;
        }
    }
    else {
        // File is already opened
        m_isOpen = true;
    }

    return m_isOpen;
}

bool FileWrite::writeChannelData(ALP_OBJECT_ENTRY *entry, ALP_CHANNEL *channel, Data<float, float, float> *data)
{
    if (!m_isOpen) {
        return false;
    }
    return m_fileIO->writeChannelData(entry,channel,data);
}

bool FileWrite::writeStreamData(ALP_OBJECT_ENTRY *entry, const char *buf, int len)
{
    if (!m_isOpen) {
        return false;
    }
    return m_fileIO->writeStreamData(entry,buf, len);
}

bool FileWrite::updateName(int seq, QString name)
{
    if (!m_isOpen) {
        return false;
    }
    return m_fileIO->updateName(seq,name);
}

FileIO *FileWrite::fileIO() const
{
    return m_fileIO;
}

void FileWrite::setFileIO(FileIO *fileIO)
{
    m_fileIO = fileIO;
}

int FileWrite::fileType(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QString surfix = "." + fileInfo.suffix().toLower();

    if (surfix == ".axp") {
        return FileIO::AXPFile;
    }
    else if (surfix == ".alp") {
        return FileIO::ALPFile;
    }
    else {
        return FileIO::NoneFile;
    }
}

QString FileWrite::createNewFile(QString fileName, QString path, FileIO::FileType type)
{
    if (type == FileIO::ALPFile) {
        AlpIO *alp;
        alp = new AlpIO();
        QString filepath = path + "/" + fileName + ".ALP";
        if (alp->createNewAlpFile(filepath)) {
//            delete alp;
            return filepath;
        } else {
            return "";
        }
    }
}
