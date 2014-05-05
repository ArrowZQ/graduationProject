#include "filewrite.h"

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

}
