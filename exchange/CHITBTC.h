#ifndef CHITBTC_H
#define CHITBTC_H

#include <QObject>
#include "CExchange.h"

class CHITBTC : public CExchange
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
    QMap<int, QNetworkReply *>                  m_mapPendingReplies;

protected:
    QNetworkAccessManager                       *m_pNetworkAccessManager;
    EN_RESPONSE_STATUS                          m_enResponseStatus;
    QTimer                                      m_Timer;

public:
    Q_INVOKABLE                                 CHITBTC(QObject *parent = nullptr);
                                                ~CHITBTC();

    virtual void                                readFee();
    virtual void                                readBalance();
    virtual void                                readTicker();
    virtual void                                buy(QString , double, double, int);
    virtual void                                sell(QString, double, double, int);

protected:
    QNetworkReply *                             callAPI(HTTP_METHOD enHttpMethod, QString strPath, bool isAuth = false, QByteArray btarrayData = nullptr);
    bool                                        isSucceedCallingAPI(QJsonObject jsonObject);

signals:

public slots:
    void                                        onReceivedFee();
    void                                        onReceivedBalance();
    void                                        onReceivedTicker();
    void                                        onReceivedBuyingBTCResult();
    void                                        onReceivedSellingBTCResult();

    void                                        onTimer();
};

#endif // CHITBTC_H
