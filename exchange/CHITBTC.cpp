#include "CHITBTC.h"
#include <QtWidgets>
#include <QMessageBox>

CHITBTC::CHITBTC(QObject *parent) : CExchange(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    m_enResponseStatus = enInitial;
    m_Timer.setInterval(1000);
    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

CHITBTC::~CHITBTC()
{
    m_Timer.stop();
    if (m_pNetworkAccessManager) {
        delete m_pNetworkAccessManager;
        m_pNetworkAccessManager = nullptr;
    }
}

void CHITBTC::readFee()
{
    QString strSymbol = "BTCUSD";
    QNetworkReply *pReply = callAPI(GET, "/api/2/trading/fee/" + strSymbol, true);
    m_mapPendingReplies.insert(enGetFee, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedFee()));
}

void CHITBTC::readBalance()
{
    QNetworkReply *pReply = callAPI(GET, "/api/2/trading/balance", true);
    m_mapPendingReplies.insert(enGetBalanceInfo, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedBalance()));
}

void CHITBTC::readTicker()
{
    QNetworkReply *pReply = callAPI(GET, "ticker/BTCUSD");
    m_mapPendingReplies.insert(enGetTicker, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedTicker()));
}

void CHITBTC::buy(QString strSymbol, double dfBtcToBuy, double dfPriceTobuyPerCoin, int nID)
{
    strSymbol = "BTCUSD";
    QByteArray btarrayData = "symbol=" + strSymbol.toLower().toLatin1() + "&side=buy&quantity=" + byteArrayFromDouble(dfBtcToBuy) + "&price=" + byteArrayFromDouble(dfPriceTobuyPerCoin);
    QNetworkReply *pReply = callAPI(POST, "/api/2/order", true, btarrayData);

    QString strKey = QString("%1%2").arg(enBuy).arg(nID);
    int nKey = strKey.toInt();
    m_mapPendingReplies.insert(nKey, pReply);

    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedBuyingBTCResult()));
}

void CHITBTC::sell(QString strSymbol, double dfBtcToSell, double dfPriceToSellPerCoin, int nID)
{
    strSymbol = "BTCUSD";
    QByteArray btarrayData = "symbol=" + strSymbol.toLower().toLatin1() + "&side=sell&quantity=" + byteArrayFromDouble(dfBtcToSell) + "&price=" + byteArrayFromDouble(dfPriceToSellPerCoin);
    QNetworkReply *pReply = callAPI(POST, "/api/2/order", true, btarrayData);

    QString strKey = QString("%1%2").arg(enSell).arg(nID);
    int nKey = strKey.toInt();
    m_mapPendingReplies.insert(nKey, pReply);

    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedSellingBTCResult()));
}

QNetworkReply * CHITBTC::callAPI(HTTP_METHOD enHttpMethod, QString strPath, bool isAuth, QByteArray btarrayData)
{
    QNetworkReply *pReply = nullptr;
    QNetworkRequest request;
    QUrl url = m_APIURL;

    if (isAuth) {
        QByteArray btarrayAuthToken = m_btarrayAPIKey + ':' + m_btarraySecKey;
        btarrayAuthToken = btarrayAuthToken.toBase64();
        qDebug() << "Auth token :" << btarrayAuthToken;
        request.setRawHeader("Authorization", " Basic " + btarrayAuthToken);
    }
    else {
        strPath = "/api/2/public/" + strPath;
        enHttpMethod = GET;
    }

    url.setPath(strPath);
    request.setUrl(url);

    QUrlQuery params = QUrlQuery(btarrayData);
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

bool CHITBTC::isSucceedCallingAPI(QJsonObject jsonObject)
{
    if (!jsonObject.contains("error"))
        return true;
    return false;
}

// slots
void CHITBTC::onReceivedFee()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetFee];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        qDebug() << "recevied HITBTC Fee info" << btarrayRespData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        if (!isSucceedCallingAPI(jsonObject)) {
            jsonObject = jsonObject["error"].toObject();
            QString strErrorMessage = tr("Fail to get Fee on HITBTC.\r\nError:%1").arg(jsonObject["description"].toString());
            QMessageBox::warning(nullptr, tr("Warning"), strErrorMessage);
        } else {
            QJsonValue jsonTakerFee = jsonObject["takeLiquidityRate"];
            QJsonValue jsonMakerFee = jsonObject["provideLiquidityRate"];
            if (!jsonTakerFee.isNull() && !jsonMakerFee.isNull()) {
                double dfTakerFee = jsonTakerFee.toString().toDouble();
                double dfMakerFee = jsonMakerFee.toString().toDouble();
                double dfFee = qMax(dfTakerFee, dfMakerFee) * 100;
                m_dfFee = dfFee;
                emit receivedTradingFee(dfFee);
            }
            m_enResponseStatus = enReceivedFee;
        }

        m_mapPendingReplies.remove(enGetFee);
        pReply->deleteLater();
    }
}

void CHITBTC::onReceivedBalance()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetBalanceInfo];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        qDebug() << "recevied HITBTC balance info" << btarrayRespData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        if (!isSucceedCallingAPI(jsonObject)) {
            jsonObject = jsonObject["error"].toObject();
            QString strErrorMessage = tr("Fail to get balance on HITBTC.\r\nError:%1").arg(jsonObject["description"].toString());
            QMessageBox::warning(nullptr, tr("Warning"), strErrorMessage);
        } else {
            QJsonArray jsonArray = jsonDoc.array();
            foreach(const QJsonValue &value, jsonArray) {
                QJsonObject jsonObject = value.toObject();
                if (jsonObject["currency"].toString().toUpper() == "BTC") {
                    m_dfCur1Balance = jsonObject["available"].toString().toDouble();
                }
                if (jsonObject["currency"].toString().toUpper() == "USD") {
                    m_dfCur2Balance = jsonObject["available"].toString().toDouble();
                }

            }
            emit receivedBalance(m_dfCur1Balance, m_dfCur2Balance);
            m_enResponseStatus = enReceivedBalanceInfo;
        }

        m_mapPendingReplies.remove(enGetBalanceInfo);
        pReply->deleteLater();
    }
}

void CHITBTC::onReceivedBuyingBTCResult()
{
    QNetworkReply *pReply = nullptr;
    int nKey = -1, nID = -1;
    QMapIterator<int, QNetworkReply *> iterator(m_mapPendingReplies);
    while (iterator.hasNext()) {
        iterator.next();
        nKey = iterator.key();
        QString strKey = QString::number(nKey);
        QString strPrefix = QString::number(enBuy);
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
            jsonObject = jsonObject["error"].toObject();
            QString strError = jsonObject["description"].toString();
            emit errorRequestBuyOrder(-1, strError, nID);
            //QString strErrorMessage = tr("Fail to order to buy HITBTC.\r\nError:%1").arg(strError);
            //QMessageBox::warning(nullptr, tr("Warning"), strErrorMessage);
        } else {
            emit succeedOrderingBTC(this, nID, BUY);
            //emit succeedBuyingBTC(this, nID);
        }

        //m_mapPendingReplies.remove(enBuy);
        m_enResponseStatus = enSucceedBuying;
        pReply->deleteLater();
    }
}

void CHITBTC::onReceivedSellingBTCResult()
{
    QNetworkReply *pReply = nullptr;
    int nKey = -1, nID = -1;
    QMapIterator<int, QNetworkReply *> iterator(m_mapPendingReplies);
    while (iterator.hasNext()) {
        iterator.next();
        nKey = iterator.key();
        QString strKey = QString::number(nKey);
        QString strPrefix = QString::number(enSell);
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
            jsonObject = jsonObject["error"].toObject();

            QString strError = jsonObject["description"].toString();
            emit errorRequestBuyOrder(-1, strError, nID);
//            QString strErrorMessage = tr("Fail to order to sell HITBTC.\r\nError:%1").arg(jsonObject["description"].toString());
//            QMessageBox::warning(nullptr, tr("Warning"), strErrorMessage);
        } else {
            emit succeedOrderingBTC(this, nID, SELL);
            //emit succeedSellingBTC(this, nID);
        }

        //m_mapPendingReplies.remove(enSell);
        m_enResponseStatus = enSucceedSelling;
        pReply->deleteLater();
    }
}

void CHITBTC::onReceivedTicker()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetTicker];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        //qDebug() << "recevied HITBTC balance info" << btarrayRespData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        QString strValue = jsonObject["symbol"].toString();
        if (strValue.toUpper() == "BTCUSD") {
            QString strBidPrice = jsonObject["bid"].toString();
            QString strAskPrice = jsonObject["ask"].toString();
            QString strHighPrice = jsonObject["high"].toString();
            QString strLowPrice = jsonObject["low"].toString();
            QString strLastPrice = jsonObject["last"].toString();
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

void CHITBTC::onTimer()
{

}
