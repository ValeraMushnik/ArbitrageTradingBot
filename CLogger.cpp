#include "CLogger.h"

CLogger::CLogger(QString strFilePath, QObject *parent):QObject(parent)
{
    m_pLogFile = new QFile(strFilePath);
    if (!m_pLogFile->open(QIODevice::WriteOnly)) {
        delete m_pLogFile;
        m_pLogFile = nullptr;
    }
}

CLogger::~CLogger()
{
    if (m_pLogFile->isOpen()) {
        m_pLogFile->close();
        delete m_pLogFile;
        m_pLogFile = nullptr;
    }
}

bool CLogger::writeLog(QString strLog)
{
    if (m_pLogFile == nullptr)
        return false;
    m_pLogFile->write(strLog.toLocal8Bit());
    m_pLogFile->write("\r\n", 2);
    return true;
}
