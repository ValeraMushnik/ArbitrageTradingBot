#include "CBinance.h"
#include <QtWidgets>
#include <QMessageBox>

CBinance::CBinance(QObject *parent) : CExchange(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    m_enResponseStatus = enInitial;
    m_nBinanceServerTime = 0;
    m_Timer.setInterval(1000);
    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

CBinance::~CBinance()
{
    m_Timer.stop();
    if (m_pNetworkAccessManager) {
        delete m_pNetworkAccessManager;
        m_pNetworkAccessManager = nullptr;
    }
}

void CBinance::getServerTime()
{
    QNetworkReply *pReply = callAPI(GET, "/api/v1/time", false);
    m_mapPendingReplies.insert(enGetServerTime, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedServerTime()));
}

void CBinance::getAccountInfo()
{
    if (m_enResponseStatus < enReceivedServerTime) {
        m_nBinanceServerTime = QDateTime::currentDateTime().toMSecsSinceEpoch() - 10000;
        QApplication::restoreOverrideCursor();
        qDebug() << "serverTime force to set " << m_nBinanceServerTime;
    }

    QNetworkReply *pReply = callAPI(GET, "/api/v3/account", true);
    m_mapPendingReplies.insert(enGetAccountInfo, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onreceivedBalance()));
}

void CBinance::readFee()
{
    return readBalance();
}

void CBinance::readBalance()
{
    if (m_nBinanceServerTime == 0) {
        getServerTime();
        QObject::connect(this, SIGNAL(receivedServerTime()), this, SLOT(getAccountInfo()));
    } else {
        getAccountInfo();
    }
}

void CBinance::readTicker()
{
    QNetworkReply *pReply = callAPI(GET, "/api/v1/ticker/24hr", false, "symbol=BTCUSDT");
    m_mapPendingReplies.insert(enGetTicker, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedTicker()));
}

void CBinance::buy(QString strSymbol, double dfBtcToBuy, double dfPriceTobuyPerCoin, int nID)
{
    strSymbol = "BTCUSDT";
    QByteArray btarrayData = "symbol=" + strSymbol.toLatin1() + "&side=BUY&type=LIMIT&timeInForce=GTC&quantity=" + QByteArray::number(dfBtcToBuy) + "&price=" + QByteArray::number(dfPriceTobuyPerCoin) + "&";
    QNetworkReply *pReply = callAPI(POST, "/api/v3/order", true, btarrayData);
    QString strKey = QString("%1%2").arg(enBuyBtc).arg(nID);
    int nKey = strKey.toInt();
    m_mapPendingReplies.insert(nKey, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedBuyingBTCResult()));
}

void CBinance::sell(QString strSymbol, double dfBtcToSell, double dfPriceToSellPerCoin, int nID)
{
    strSymbol = "BTCUSDT";
    QByteArray btarrayData = "symbol=" + strSymbol.toLatin1() + "&side=SELL&type=LIMIT&timeInForce=GTC&quantity=" + QByteArray::number(dfBtcToSell) + "&price=" + QByteArray::number(dfPriceToSellPerCoin) + "&";
    QNetworkReply *pReply = callAPI(POST, "/api/v3/order", true, btarrayData);
    QString strKey = QString("%1%2").arg(enSellBtc).arg(nID);
    int nKey = strKey.toInt();
    m_mapPendingReplies.insert(nKey, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedSellingBTCResult()));
}

QNetworkReply * CBinance::callAPI(HTTP_METHOD enHttpMethod, QString strPath, bool isAuth, QByteArray btarrayData)
{
    QNetworkReply *pReply = nullptr;
    QNetworkRequest request;

    request.setRawHeader(QByteArray("X-MBX-APIKEY"), m_btarrayAPIKey);
    QUrl url = m_APIURL;
    url.setPath(strPath);

    QByteArray btarrayQuery = btarrayData;
    if (isAuth) {
        QByteArray btarrayParam = btarrayData + "recvWindow=30000&timestamp=" + QByteArray::number(m_nBinanceServerTime);
        btarrayQuery = btarrayParam + "&signature=" + hmacSha256(m_btarraySecKey, btarrayParam).toHex();
    }
    else {
        btarrayQuery = btarrayData;
    }

    QUrlQuery params = QUrlQuery(btarrayQuery);
    request.setUrl(url);

    switch (enHttpMethod) {
        case GET:{
            url.setQuery(params);
            request.setUrl(url);
            pReply = m_pNetworkAccessManager->get(request);
            break;
        }
        case POST:{
            request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
            pReply = m_pNetworkAccessManager->post(request, params.query(QUrl::FullyEncoded).toLatin1());
            qDebug() << request.url().toString();
            qDebug() << params.query(QUrl::FullyEncoded).toLatin1();
            break;
        }
    }

    return pReply;
}

bool CBinance::isSucceedCallingAPI(QJsonObject jsonObject)
{
    return !jsonObject.contains("code");
}

// slots
void CBinance::onReceivedServerTime()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetServerTime];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();

        if (!isSucceedCallingAPI(jsonObject)) {
            int nCode = jsonObject["code"].toInt();
            QString strMsg = jsonObject["msg"].toString();
            QString strErrorMsg = tr("Fail calling request to get Binanace server time. code = %1, msg = %2").arg(nCode).arg(strMsg);
            QMessageBox::warning(nullptr, tr("Error"), strErrorMsg);
        }

        QJsonValue jsonValue = jsonObject.value("serverTime");
        QVariant value = jsonValue.toVariant();

        qint64 nServerTime = value.toLongLong();
        m_nBinanceServerTime = nServerTime;
        m_mapPendingReplies.remove(enGetServerTime);
        m_enResponseStatus = enReceivedServerTime;
        QString strLogMessage = tr("Received Binanace server time = %1").arg(nServerTime);
        qDebug() << strLogMessage;
        emit receivedServerTime();
        pReply->deleteLater();

        if (!m_Timer.isActive())
            m_Timer.start();
    }
}

void CBinance::onreceivedBalance()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetAccountInfo];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        qDebug() << jsonObject;

        if (!isSucceedCallingAPI(jsonObject) || !jsonObject.contains("balances")) {
            int nCode = jsonObject["code"].toInt();
            QString strMsg = jsonObject["msg"].toString();
            QString strErrorMsg = tr("Fail calling request to get Binanace account info. code = %1, msg = %2").arg(nCode).arg(strMsg);
            QMessageBox::warning(nullptr, tr("Error"), strErrorMsg);
        } else {
            QJsonValue jsonValue = jsonObject.value("balances");
            QJsonValue jsonMakerFee = jsonObject.value("makerCommission");
            QJsonValue jsonTakerFee = jsonObject.value("takerCommission");

            if (!jsonValue.isNull()) {
                QJsonArray jsonArray = jsonValue.toArray();
                foreach(const QJsonValue &value, jsonArray) {
                    jsonObject = value.toObject();
                    jsonValue = jsonObject["asset"];
                    if (!jsonValue.isNull()) {
                        QString strAssets = jsonValue.toString().toUpper();
                        double dfBalance = jsonObject["free"].toString().toDouble();
                        if ( strAssets == "BTC") {
                            m_dfCur1Balance = dfBalance;
                        } else if (strAssets == "USDT") {
                            m_dfCur2Balance = dfBalance;
                        }
                    }
                }
                emit receivedBalance(m_dfCur1Balance, m_dfCur2Balance);
            }

            if (!jsonMakerFee.isNull() && !jsonTakerFee.isNull()) {
                double dfMakerFee = jsonMakerFee.toDouble();
                double dfTakerFee = jsonTakerFee.toDouble();
                double dfFee = qMax(dfMakerFee, dfTakerFee) / 100;
                m_dfFee = dfFee;
                emit receivedTradingFee(dfFee);
            }

        }
        m_mapPendingReplies.remove(enGetAccountInfo);
        m_enResponseStatus = enReceivedBalance;
        pReply->deleteLater();
    }
}

void CBinance::onReceivedBuyingBTCResult()
{
    QNetworkReply *pReply = nullptr;
    int nKey = -1, nID = -1;
    QMapIterator<int, QNetworkReply *> iterator(m_mapPendingReplies);
    while (iterator.hasNext()) {
        iterator.next();
        nKey = iterator.key();
        QString strKey = QString::number(nKey);
        QString strPrefix = QString::number(enBuyBtc);
        if (strKey.startsWith(strPrefix)) {
            QNetworkReply *pValue = iterator.value();
            if (pValue->isFinished() && pValue->isReadable()) {
                pReply = pValue;
                QString strID = strKey.right(strKey.length() - strPrefix.length());
                nID = strID.toInt();
                m_mapPendingReplies.remove(nKey);
                break;
            }
        }
    }

    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        qDebug() << jsonObject;

        if (!isSucceedCallingAPI(jsonObject)) {
            int nCode = jsonObject["code"].toInt();
            QString strMsg = jsonObject["msg"].toString();
            emit errorRequestBuyOrder(nCode, strMsg, nID);
            //QString strErrorMsg = tr("Fail calling request to buy Binance BTC. code = %1, msg = %2").arg(nCode).arg(strMsg);
            //QMessageBox::warning(nullptr, tr("Error"), strErrorMsg);
        } else {
            emit succeedOrderingBTC(this, nID, BUY);
            //emit succeedBuyingBTC(this, nID);
        }
        //m_mapPendingReplies.remove(nKey);
        m_enResponseStatus = enSucceedBuying;
        pReply->deleteLater();
    }
}

void CBinance::onReceivedSellingBTCResult()
{
    QNetworkReply *pReply = nullptr;
    int nKey = -1, nID = -1;
    QMapIterator<int, QNetworkReply *> iterator(m_mapPendingReplies);
    while (iterator.hasNext()) {
        iterator.next();
        nKey = iterator.key();
        QString strKey = QString::number(nKey);
        QString strPrefix = QString::number(enSellBtc);
        if (strKey.startsWith(strPrefix)) {
            QNetworkReply *pValue = iterator.value();
            if (pValue->isFinished() && pValue->isReadable()) {
                pReply = pValue;
                QString strID = strKey.right(strKey.length() - strPrefix.length());
                nID = strID.toInt();
                m_mapPendingReplies.remove(nKey);
                break;
            }
        }
    }

    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        qDebug() << jsonObject;

        if (!isSucceedCallingAPI(jsonObject)) {
            int nCode = jsonObject["code"].toInt();
            QString strMsg = jsonObject["msg"].toString();
            emit errorRequestSellOrder(nCode, strMsg, nID);
            //QString strErrorMsg = tr("Fail calling request to sell Binance BTC. code = %1, msg = %2").arg(nCode).arg(strMsg);
            //QMessageBox::warning(nullptr, tr("Error"), strErrorMsg);
        } else {
            emit succeedOrderingBTC(this, nID, SELL);
            //emit succeedSellingBTC(this, nID);
        }

        //m_mapPendingReplies.remove(enSellBtc);
        m_enResponseStatus = enSucceedSelling;
        pReply->deleteLater();
    }
}

void CBinance::onReceivedTicker()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetTicker];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        //qDebug() << jsonObject;
        if (!isSucceedCallingAPI(jsonObject) || !jsonObject.contains("bidPrice") || !jsonObject.contains("askPrice")) {
            int nCode = jsonObject["code"].toInt();
            QString strMsg = jsonObject["msg"].toString();
            QString strErrorMsg = tr("Fail calling request to get ticker. code = %1, msg = %2").arg(nCode).arg(strMsg);
            //QMessageBox::warning(this, tr("Error"), strErrorMsg);
        } else {
            QString strBidPrice = jsonObject["bidPrice"].toString();
            QString strAskPrice = jsonObject["askPrice"].toString();
            QString strHighPrice = jsonObject["highPrice"].toString();
            QString strLowPrice = jsonObject["lowPrice"].toString();
            QString strLastPrice = jsonObject["lastPrice"].toString();
            QString strVolume = jsonObject["volume"].toString();

            m_dfSellPrice = strBidPrice.toDouble();
            m_dfBuyPrice = strAskPrice.toDouble();
            m_dfHighPrice = strHighPrice.toDouble();
            m_dfLowPrice = strLowPrice.toDouble();
            m_dfLastPrice = strLastPrice.toDouble();
            m_dfVolume = strVolume.toDouble();

            CExchange::Ticker tagTicker;
            tagTicker.dfBuyPrice = m_dfBuyPrice;
            tagTicker.dfSellPrice = m_dfSellPrice;
            tagTicker.dfHightPrice = m_dfHighPrice;
            tagTicker.dfLowPrice = m_dfLowPrice;
            tagTicker.dfLastPrice = m_dfLastPrice;
            tagTicker.dfVolume = m_dfVolume;

            emit receivedTicker(tagTicker);
        }

        m_mapPendingReplies.remove(enGetTicker);
        m_enResponseStatus = enReceivedTicker;
        pReply->deleteLater();
    }
}

void CBinance::onTimer()
{
    if (m_nBinanceServerTime > 0)
        m_nBinanceServerTime += m_Timer.interval();
}
