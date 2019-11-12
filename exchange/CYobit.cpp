#include "CYobit.h"
#include <QMessageBox>

CYobit::CYobit(QObject *parent) : CExchange(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    m_enResponseStatus = enInitial;
    m_nPrivateNonce = (QDateTime::currentDateTime().toTime_t() - 1371854884) * 10;
    m_Timer.setInterval(1000);
    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}


void CYobit::readFee()
{
    QNetworkReply *pReply = callAPI(GET, "info", false);
    m_mapPendingReplies.insert(enGetFee, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedFee()));
}

void CYobit::readBalance()
{
    QNetworkReply *pReply = callAPI(POST, "", true, "method=getInfo&");
    m_mapPendingReplies.insert(enGetBalanceInfo, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedBalance()));
}

void CYobit::readTicker()
{
    QNetworkReply *pReply = callAPI(GET, "ticker/btc_usd", false);
    m_mapPendingReplies.insert(enGetTicker, pReply);
    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedTicker()));

}

void CYobit::buy(QString strSymbol, double dfAmount, double dfPrice, int nID)
{
    strSymbol = "btc_usd";
    QByteArray btarrayData = "method=Trade&pair=" + strSymbol.toLatin1() + "&type=buy&rate=" +
    CExchange::byteArrayFromDouble(dfPrice, 8, 0) + "&amount=" +
                          CExchange::byteArrayFromDouble(dfAmount, 8, 0) + "&";

    QNetworkReply *pReply = callAPI(POST, "", true, btarrayData);

    QString strKey = QString("%1%2").arg(enBuy).arg(nID);
    int nKey = strKey.toInt();
    m_mapPendingReplies.insert(nKey, pReply);

    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedBuyingBTCResult()));
}

void CYobit::sell(QString strSymbol, double dfAmount, double dfPrice, int nID)
{
    strSymbol = "btc_usd";
    QByteArray btarrayData = "method=Trade&pair=" + strSymbol.toLatin1() + "&type=sell&rate=" +
    CExchange::byteArrayFromDouble(dfPrice, 8, 0) + "&amount=" +
                          CExchange::byteArrayFromDouble(dfAmount, 8, 0) + "&";

    QNetworkReply *pReply = callAPI(POST, "", true, btarrayData);

    QString strKey = QString("%1%2").arg(enSell).arg(nID);
    int nKey = strKey.toInt();
    m_mapPendingReplies.insert(nKey, pReply);

    QObject::connect(pReply, SIGNAL(finished()), this, SLOT(onReceivedSellingBTCResult()));
}

QNetworkReply * CYobit::callAPI(HTTP_METHOD enHttpMethod, QString strPath, bool isAuth, QByteArray btarrayData)
{
    QNetworkReply *pReply = nullptr;
    QByteArray btarrayQuery;
    QNetworkRequest request;
    QUrl url;

    url = m_APIURL;

    request.setRawHeader("Key", m_btarrayAPIKey);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    if (isAuth) {
        btarrayQuery = btarrayData + "nonce=" + QByteArray::number(++m_nPrivateNonce);
        strPath = "/tapi/" + strPath;
        request.setRawHeader("Sign", hmacSha512(m_btarraySecKey, btarrayQuery).toHex());
    }
    else {
        if (btarrayData.isEmpty()) {
            enHttpMethod = GET;
            strPath = "/api/3/" + strPath;
        }
        else {
            enHttpMethod = POST;
            strPath = "/api/3/" + strPath;
            btarrayQuery = btarrayData;
        }
    }

    url.setPath(strPath);
    request.setUrl(url);
    QUrlQuery params = QUrlQuery(btarrayQuery);

    switch (enHttpMethod) {
    case GET:
        pReply = m_pNetworkAccessManager->get(request);
        break;
    case POST:
        pReply = m_pNetworkAccessManager->post(request, params.query(QUrl::FullyEncoded).toLatin1());
        break;
    }

    return pReply;
}

bool CYobit::isSucceedCallingAPI(QJsonObject jsonObject)
{
    if (jsonObject.contains("success")) {
        if (jsonObject["success"].toInt() == 1)
            return true;
        else
            return false;
    }

    return true;
}

void CYobit::onReceivedBalance()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetBalanceInfo];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        qDebug() << "recevied Yobit balance info" << btarrayRespData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        if (!isSucceedCallingAPI(jsonObject) || btarrayRespData.startsWith("<")) {
            QString strErrorMessage = tr("Fail to get balance on Yobit.\r\nError:%1").arg(jsonObject["error"].toString());
            QMessageBox::warning(nullptr, tr("Warning"), strErrorMessage);
        } else {
            QJsonObject jsonReturn = jsonObject["return"].toObject();
            QJsonObject jsonFunds = jsonReturn["funds"].toObject();
            double dfBalance1 = jsonFunds["btc"].toDouble(0.0);
            double dfBalance2 = jsonFunds["usd"].toDouble(0.0);
            m_dfCur1Balance = dfBalance1;
            m_dfCur2Balance = dfBalance2;
            emit receivedBalance(m_dfCur1Balance, m_dfCur2Balance);
            m_enResponseStatus = enReceivedBalanceInfo;
        }

        m_mapPendingReplies.remove(enGetBalanceInfo);
        pReply->deleteLater();
    }
}

void CYobit::onReceivedFee()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetFee];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        qDebug() << "recevied CYobit fee info" << btarrayRespData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        if (!isSucceedCallingAPI(jsonObject) || btarrayRespData.startsWith("<")) {
            QString strErrorMessage = tr("Fail to get Fee on Yobit.\r\nError:%1").arg(jsonObject["error"].toString());
            QMessageBox::warning(nullptr, tr("Warning"), strErrorMessage);
        } else {
            QJsonObject jsonPairs = jsonObject["pairs"].toObject();
            QJsonObject jsonPair = jsonPairs["btc_usd"].toObject();
            m_dfFee = jsonPair["fee"].toDouble();
            emit receivedTradingFee(m_dfFee);
            m_enResponseStatus = enReceivedFee;
        }

        m_mapPendingReplies.remove(enGetFee);
        pReply->deleteLater();
    }
}

void CYobit::onReceivedTicker()
{
    QNetworkReply *pReply = m_mapPendingReplies[enGetTicker];
    if (pReply) {
        QByteArray btarrayRespData = pReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(btarrayRespData);
        QJsonObject jsonObject = jsonDoc.object();
        if (!isSucceedCallingAPI(jsonObject) || btarrayRespData.startsWith("<")) {
            QString strErrorMessage = tr("Fail to get ticker on Yobit.\r\nError:%1").arg(jsonObject["error"].toString());
            QMessageBox::warning(nullptr, tr("Warning"), strErrorMessage);
        } else {
            QJsonObject jsonPair = jsonObject["btc_usd"].toObject();
            m_dfHighPrice = jsonPair["high"].toDouble();
            m_dfLowPrice = jsonPair["low"].toDouble();
            m_dfVolume = jsonPair["vol"].toDouble();
            m_dfLastPrice = jsonPair["last"].toDouble();
            m_dfBuyPrice = jsonPair["sell"].toDouble();
            m_dfSellPrice = jsonPair["buy"].toDouble();

            CExchange::Ticker tagTicker;
            tagTicker.dfBuyPrice = m_dfBuyPrice;
            tagTicker.dfSellPrice = m_dfSellPrice;
            tagTicker.dfHightPrice = m_dfHighPrice;
            tagTicker.dfLowPrice = m_dfLowPrice;
            tagTicker.dfLastPrice = m_dfLastPrice;
            tagTicker.dfVolume = m_dfVolume;

            emit receivedTicker(tagTicker);
            m_enResponseStatus = enReceivedTicker;
        }
        m_mapPendingReplies.remove(enGetTicker);
        pReply->deleteLater();
    }
}

void CYobit::onReceivedBuyingBTCResult()
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

        if (!isSucceedCallingAPI(jsonObject)  || btarrayRespData.startsWith("<")) {
            QString strErrorMessage = tr("Fail to get buy btc on Yobit.\r\nError:%1").arg(jsonObject["error"].toString());
            emit errorRequestBuyOrder(-1, strErrorMessage, nID);
        } else {
            emit succeedOrderingBTC(this, nID, BUY);
        }

        //m_mapPendingReplies.remove(enBuy);
        m_enResponseStatus = enSucceedBuying;
        pReply->deleteLater();
    }
}

void CYobit::onReceivedSellingBTCResult()
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

        if (!isSucceedCallingAPI(jsonObject)  || btarrayRespData.startsWith("<")) {
            QString strErrorMessage = tr("Fail to get sell btc on Yobit.\r\nError:%1").arg(jsonObject["error"].toString());
            emit errorRequestSellOrder(-1, strErrorMessage, nID);
        } else {
            emit succeedOrderingBTC(this, nID, BUY);
        }

        //m_mapPendingReplies.remove(enBuy);
        m_enResponseStatus = enSucceedSelling;
        pReply->deleteLater();
    }
}

void CYobit::onTimer()
{

}
