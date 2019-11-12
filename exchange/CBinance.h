#ifndef CBINANCE_H
#define CBINANCE_H

#include <QObject>
#include "CExchange.h"
#include <QtNetwork>

class CBinance : public CExchange
{
    Q_OBJECT

public:

    typedef enum {
        enGetServerTime = 0,
        enGetAccountInfo,
        enGetTicker,
        enBuyBtc,
        enSellBtc
    } EN_REQUEST;

    typedef enum {
        enInitial = 0,
        enReceivedServerTime,
        enReceivedBalance,
        enReceivedTicker,
        enSucceedBuying,
        enSucceedSelling
    }EN_RESPONSE_STATUS;

private:

    QMap<int, QNetworkReply *>                  m_mapPendingReplies;

protected:

    qint64                                      m_nBinanceServerTime;
    QNetworkAccessManager                       *m_pNetworkAccessManager;

    EN_RESPONSE_STATUS                          m_enResponseStatus;
    QTimer                                      m_Timer;

public:
    Q_INVOKABLE                                 CBinance(QObject *parent = nullptr);
                                                ~CBinance();

    virtual void                                readFee();
    virtual void                                readBalance();
    virtual void                                readTicker();
    virtual void                                buy(QString , double, double, int nID = -1);
    virtual void                                sell(QString, double, double, int nID = -1);

protected:

    QNetworkReply *                             callAPI(HTTP_METHOD enHttpMethod, QString strPath, bool isAuth, QByteArray btarrayData = nullptr);
    bool                                        isSucceedCallingAPI(QJsonObject jsonObject);

signals:

    void                                        receivedServerTime();
public slots:

    void                                        getServerTime();
    void                                        getAccountInfo();

    void                                        onReceivedServerTime();
    void                                        onreceivedBalance();
    void                                        onReceivedTicker();
    void                                        onReceivedBuyingBTCResult();
    void                                        onReceivedSellingBTCResult();

    void                                        onTimer();
};

#endif // CBINANCE_H
