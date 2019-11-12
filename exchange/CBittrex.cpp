#include "CBittrex.h"
#include <QMessageBox>

CBittrex::CBittrex(QObject *parent) : CExchange(parent)
{

}

void CBittrex::readFee()
{

}

void CBittrex::readBalance()
{

}

void CBittrex::readTicker()
{
    QNetworkReply *pReply = callAPI("getmarketsummary?market=usd-btc");
    m_mapPendingReplies.insert(enGetTicker, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedTicker()));
}

void CBittrex::buy(QString , double, double, int)
{

}

void CBittrex::sell(QString, double, double, int)
{

}

QNetworkReply * CBittrex::callAPI(QString strPath, bool isAuth)
{
    QNetworkRequest request;
    QNetworkReply *pReply = nullptr;

    QUrl url = m_APIURL;

    QByteArray btarrayParam;
    if (isAuth) {
        strPath = "/api/v1.1/" + strPath + "apikey=" + m_btarrayAPIKey + "&nonce=" + QByteArray::number(++m_nBittrexNonce);
        QString strUrl = url.url();
        QByteArray btarraySign = hmacSha512(m_btarraySecKey, strUrl).toHex();
        qDebug() << "apisign" << btarraySign;
        request.setRawHeader("apisign", btarraySign);
    }
    else {
        strPath = "/api/v1.1/public/" + strPath;
    }

    url.setPath(strPath);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "Application/json");
    QString strLog = "Bittrex API : " + url.url();
    qDebug() << strLog;
    pReply = m_pNetworkAccessManager->get(request);
    return pReply;
}

bool CBittrex::isSucceedCallingAPI(QJsonObject jsonObject)
{
    return jsonObject["success"].toBool();
}

void CBittrex::onReceivedFee()
{

}

void CBittrex::onReceivedBalance()
{

}

void CBittrex::onReceivedTicker()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetTicker];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        //qDebug() << jsonObject;
        if (!isSucceedCallingAPI(jsonObject)) {
            QString strMsg = jsonObject["message"].toString();
            QString strErrorMsg = tr("Fail calling request to get ticker. msg = %1").arg(strMsg);
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

void CBittrex::onReceivedBuyingBTCResult()
{

}

void CBittrex::onReceivedSellingBTCResult()
{

}
