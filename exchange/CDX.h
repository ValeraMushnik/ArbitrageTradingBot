#ifndef CDX_H
#define CDX_H

#include <QObject>
#include "CExchange.h"

class CDX : public CExchange
{
    Q_OBJECT
public:
    Q_INVOKABLE CDX(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CDX_H
