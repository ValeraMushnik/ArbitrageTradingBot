#ifndef CBITTREX_H
#define CBITTREX_H

#include <QObject>
#include "CExchange.h"
#include <QtNetwork>

class CBittrex : public CExchange
{
    Q_OBJECT

public:

    typedef enum {
        enGetBalanceInfo = 0,
        enGetFee,
        enGetTicker,
        enBuy,
        enSell
    }EN_REQUEST;

    typedef enum {
        enInitial = 0,
        enReceivedBalanceInfo,
        enReceivedFee,
        enReceivedTicker,
        enSucceedBuying,
        enSucceedSelling
    }EN_RESPONSE_STATUS;

private:
    QNetworkAccessManager                       *m_pNetworkAccessManager;
    QMap<int, QNetworkReply *>                  m_mapPendingReplies;
    EN_RESPONSE_STATUS                          m_enResponseStatus;

protected:
    qint64                                      m_nBittrexNonce;

public:
    Q_INVOKABLE                                 CBittrex(QObject *parent = nullptr);

    virtual void                                readFee();
    virtual void                                readBalance();
    virtual void                                readTicker();
    virtual void                                buy(QString , double, double, int nID = -1);
    virtual void                                sell(QString, double, double, int nID = -1);

protected:
    QNetworkReply *                             callAPI(QString strPath, bool isAuth = false);
    bool                                        isSucceedCallingAPI(QJsonObject jsonObject);

signals:

public slots:
    void                                        onReceivedFee();
    void                                        onReceivedBalance();
    void                                        onReceivedTicker();
    void                                        onReceivedBuyingBTCResult();
    void                                        onReceivedSellingBTCResult();
};

#endif // CBITTREX_H
