#ifndef CLOGGER_H
#define CLOGGER_H

#include <QObject>
#include <QFile>

class CLogger : public QObject
{
    Q_OBJECT
protected:
    QFile                       *m_pLogFile;

public:
                                CLogger(QString strFilePath, QObject *parent = nullptr);
                                ~CLogger();
    bool                        writeLog(QString strLog);
signals:

public slots:
};

#endif // CLOGGER_H
