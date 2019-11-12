#include "CExchange.h"

CExchange::CExchange(QObject *parent) : QObject(parent)
{
    m_dfCur1Balance = 0;
    m_dfCur2Balance = 0;

    m_dfBuyPrice = 0;
    m_dfSellPrice = 0;
    m_dfHighPrice = 0;
    m_dfLowPrice = 0;
    m_dfLastPrice = 0;
    m_dfVolume = 0;
    m_dfFee = 0;
}

void CExchange::setPlatformName(QString strName)
{
    m_strPlatformName = strName;
}

void CExchange::setAPIURL(QUrl url)
{
    m_APIURL = url;
}

void CExchange::setAPIKey(QByteArray btarrayAPIKey)
{
    m_btarrayAPIKey = btarrayAPIKey;
}

void CExchange::setSecKey(QByteArray btarraySecKey)
{
    m_btarraySecKey = btarraySecKey;
}

void CExchange::setCur1Balance(double dfValue)
{
    m_dfCur1Balance = dfValue;
}

void CExchange::setCur2Balance(double dfValue)
{
    m_dfCur2Balance = dfValue;
}

void CExchange::setBuyPrice(double dfValue)
{
    m_dfBuyPrice = dfValue;
}

void CExchange::setSellPrice(double dfValue)
{
    m_dfSellPrice = dfValue;
}

void CExchange::setHightPrice(double dfValue)
{
    m_dfHighPrice = dfValue;
}

void CExchange::setLowPrice(double dfValue)
{
    m_dfLowPrice = dfValue;
}

void CExchange::setLastPrice(double dfValue)
{
    m_dfLowPrice = dfValue;
}

void CExchange::setVolume(double dfValue)
{
    m_dfVolume = dfValue;
}

void CExchange::setCurrentSymbol(QString strSymbol)
{
    m_strCurrentSymbol = strSymbol;
}

QString CExchange::getPlatformName()
{
    return m_strPlatformName;
}

QUrl CExchange::getAPIURL()
{
    return m_APIURL;
}

QByteArray CExchange::getAPIKey()
{
    return m_btarrayAPIKey;
}

QByteArray CExchange::getSecKey()
{
    return m_btarraySecKey;
}

double CExchange::getFee()
{
    return m_dfFee;
}

double CExchange::getCur1Balance()
{
    return m_dfCur1Balance;
}

double CExchange::getCur2Balance()
{
    return m_dfCur2Balance;
}

double CExchange::getBuyPrice()
{
    return m_dfBuyPrice;
}

double CExchange::getSellPrice()
{
    return m_dfSellPrice;
}

double CExchange::getHighPrice()
{
    return m_dfHighPrice;
}

double CExchange::getLowPrice()
{
    return m_dfLowPrice;
}

double CExchange::getLastPrice()
{
    return m_dfLastPrice;
}

double CExchange::getVolume()
{
    return m_dfVolume;
}

QString CExchange::getCurrentSymbol()
{
    return m_strCurrentSymbol;
}

void CExchange::readFee()
{

}

void CExchange::readBalance()
{

}

void CExchange::readTicker()
{

}

void CExchange::buy(QString , double, double, int)
{

}

void CExchange::sell(QString, double, double, int)
{

}

QByteArray CExchange::byteArrayFromDouble(const double& val, int maxDecimals, int minDecimals)
{
    if (maxDecimals > 8)
        maxDecimals = 8;

    if (maxDecimals < 0)
        maxDecimals = 0;

    if (minDecimals > maxDecimals)
        minDecimals = maxDecimals;

    if (minDecimals < 0)
        minDecimals = 0;

    int floorLength = QByteArray::number(floor(val), 'f', 0).length();
    int decimals = 16 - floorLength;

    if (val < 0)
        decimals++;

    if (decimals < minDecimals)
        decimals = minDecimals;

    QByteArray numberText = QByteArray::number(val, 'f', decimals);

    int resultLength = floorLength + maxDecimals + 1;

    if (numberText.length() > resultLength)
        numberText.chop(numberText.length() - resultLength);

    int minLength = floorLength;

    if (minDecimals > 0)
        minLength += minDecimals + 1;

    while (numberText[numberText.length() - 1] == '0' && numberText.length() > minLength)
        numberText.chop(1);

    if (numberText[numberText.length() - 1] == '.')
        numberText.chop(1);

    return numberText;
}
