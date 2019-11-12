#ifndef CEXCHANGE_H
#define CEXCHANGE_H

#include <QObject>
#include <QtNetwork>
#include "openssl/hmac.h"
#include "openssl/evp.h"

#define hmacSha256(key, baseString) QByteArray(reinterpret_cast<const char *>(HMAC(EVP_sha256(),key.constData(), key.size(), reinterpret_cast<const unsigned char *>(baseString.constData()), static_cast<size_t>(baseString.size()), nullptr, nullptr)),32)
#define hmacSha512(key, baseString) QByteArray(reinterpret_cast<const char *>(HMAC(EVP_sha512(),key.constData(), key.size(), reinterpret_cast<const unsigned char *>(baseString.constData()), static_cast<size_t>(baseString.size()), nullptr, nullptr)),64)

class CExchange : public QObject
{
    Q_OBJECT

public:
    typedef enum {
        GET = 0,
        POST
    }HTTP_METHOD;

    typedef enum {
        BUY = 0,
        SELL
    }OrderType;

    typedef struct {
        double dfBuyPrice;
        double dfSellPrice;
        double dfLastPrice;
        double dfHightPrice;
        double dfLowPrice;
        double dfVolume;
    }Ticker;

protected:
    QString                                 m_strPlatformName;
    QUrl                                    m_APIURL;
    QByteArray                              m_btarrayAPIKey;
    QByteArray                              m_btarraySecKey;

    double                                  m_dfCur1Balance;
    double                                  m_dfCur2Balance;

    double                                  m_dfBuyPrice;
    double                                  m_dfSellPrice;
    double                                  m_dfHighPrice;
    double                                  m_dfLowPrice;
    double                                  m_dfLastPrice;
    double                                  m_dfVolume;

    double                                  m_dfFee;

    QString                                 m_strCurrentSymbol;

public:
    Q_INVOKABLE                             CExchange(QObject *parent = nullptr);

    void                                    setPlatformName(QString);
    void                                    setAPIURL(QUrl url);
    void                                    setAPIKey(QByteArray);
    void                                    setSecKey(QByteArray);

    void                                    setCur1Balance(double);
    void                                    setCur2Balance(double);

    void                                    setBuyPrice(double);
    void                                    setSellPrice(double);
    void                                    setHightPrice(double);
    void                                    setLowPrice(double);
    void                                    setLastPrice(double);
    void                                    setVolume(double);

    void                                    setCurrentSymbol(QString);

    QString                                 getPlatformName();
    QUrl                                    getAPIURL();
    QByteArray                              getAPIKey();
    QByteArray                              getSecKey();

    double                                  getCur1Balance();
    double                                  getCur2Balance();

    double                                  getFee();
    double                                  getBuyPrice();
    double                                  getSellPrice();
    double                                  getHighPrice();
    double                                  getLowPrice();
    double                                  getLastPrice();
    double                                  getVolume();

    QString                                 getCurrentSymbol();

    static QByteArray                       byteArrayFromDouble(const double& val, int maxDecimals = 5, int minDecimals = 0);

signals:

    void                                    receivedTradingFee(double);
    void                                    receivedBalance(double, double);
    void                                    receivedTicker(CExchange::Ticker);

    void                                    succeedOrderingBTC(CExchange *, int, CExchange::OrderType);
    void                                    succeedBuyingBTC(CExchange *, int);
    void                                    succeedSellingBTC(CExchange *, int);

    void                                    errorRequestTradingFee(int, QString);
    void                                    errorRequestBalance(int, QString);
    void                                    errorRequestTicker(int, QString);
    void                                    errorRequestBuyOrder(int, QString, int);
    void                                    errorRequestSellOrder(int, QString, int);

public slots:

    virtual void                            readFee();
    virtual void                            readBalance();
    virtual void                            readTicker();
    virtual void                            buy(QString strSymbol, double dfAmount, double dfPrice, int nID = -1);
    virtual void                            sell(QString strSymbol, double dfAmount, double dfPrice, int nID = -1);

};

#endif // CEXCHANGE_H
