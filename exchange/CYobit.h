#ifndef CYOBIT_H
#define CYOBIT_H

#include <QObject>
#include "CExchange.h"
#include <QtNetwork>

class CYobit : public CExchange
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
    QMap<int, QNetworkReply *>              m_mapPendingReplies;
    qint64                                  m_nPrivateNonce;

protected:
    QNetworkAccessManager                   *m_pNetworkAccessManager;
    EN_RESPONSE_STATUS                      m_enResponseStatus;
    QTimer                                  m_Timer;

public:
    Q_INVOKABLE                             CYobit(QObject *parent = nullptr);

protected:
    QNetworkReply *                         callAPI(HTTP_METHOD enHttpMethod, QString strPath, bool isAuth = false, QByteArray btarrayData = nullptr);
    bool                                    isSucceedCallingAPI(QJsonObject jsonObject);

signals:

public slots:

    virtual void                            readFee();
    virtual void                            readBalance();
    virtual void                            readTicker();
    virtual void                            buy(QString strSymbol, double dfAmount, double dfPrice, int nID = -1);
    virtual void                            sell(QString strSymbol, double dfAmount, double dfPrice, int nID = -1);

    void                                    onReceivedBalance();
    void                                    onReceivedFee();
    void                                    onReceivedTicker();
    void                                    onReceivedBuyingBTCResult();
    void                                    onReceivedSellingBTCResult();

    void                                    onTimer();
};

#endif // CYOBIT_H
