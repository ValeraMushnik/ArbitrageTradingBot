#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "dock/dock_host.h"
#include "CAddPlatformDialog.h"
#include "CExchange.h"
#include "CBinance.h"
#include "CHITBTC.h"
#include "CDX.h"
#include "CYobit.h"
#include "CBittrex.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_pDockHost(new DockHost(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_metaObjectLookup.insert(CExchange::staticMetaObject.className(), &CExchange::staticMetaObject);
    m_metaObjectLookup.insert(CBinance::staticMetaObject.className(), &CBinance::staticMetaObject);
    m_metaObjectLookup.insert(CHITBTC::staticMetaObject.className(), &CHITBTC::staticMetaObject);
    m_metaObjectLookup.insert(CYobit::staticMetaObject.className(), &CYobit::staticMetaObject);
    m_metaObjectLookup.insert(CBittrex::staticMetaObject.className(), &CBittrex::staticMetaObject);
    m_metaObjectLookup.insert(CDX::staticMetaObject.className(), &CDX::staticMetaObject);

    QList<QDoubleSpinBox *> allSpinBoxes = this->centralWidget()->findChildren<QDoubleSpinBox*>();

    foreach (QDoubleSpinBox *pSpinBox, allSpinBoxes) {
        pSpinBox->setDecimals(5);
        pSpinBox->setMinimum(-99999999.0);
        pSpinBox->setMaximum(99999999.0);
        pSpinBox->setSingleStep(0.010000);
        pSpinBox->setAlignment(Qt::AlignRight);
    }

    ui->m_pspinBuyAmountFromA->setMinimum(0.0);
    ui->m_pspinBuyAmountFromB->setMinimum(0.0);
    ui->m_pspinStartRateAtoB->setMinimum(0.0);
    ui->m_pspinStartRateBtoA->setMinimum(0.0);
    ui->m_pspinStartRateAtoB->setMaximum(1.0);
    ui->m_pspinStartRateBtoA->setMaximum(1.0);

    ui->m_pbtnOrderAtoB->setEnabled(false);
    ui->m_pbtnOrderBtoA->setEnabled(false);

    initDocks();
    moveWidgetsToDocks();

    QSettings platformSettings("platforms.ini", QSettings::IniFormat, this);
    QStringList strlstPlatforms = platformSettings.childGroups();
    ui->m_pPlatformsListA->addItems(strlstPlatforms);
    ui->m_pPlatformsListB->addItems(strlstPlatforms);

    ui->m_pPlatformsListA->setCurrentText("Binance");
    ui->m_pPlatformsListB->setCurrentText("HITBTC");

    m_pExchangeA = nullptr;
    m_pExchangeB = nullptr;
    m_isReceivedPlatformABalance = false;
    m_isReceivedPlatformBBalance = false;

    QObject::connect(ui->m_pPlatformsListA, SIGNAL(currentTextChanged(QString)), this, SLOT(onChangePlatformA(QString)));
    QObject::connect(ui->m_pPlatformsListB, SIGNAL(currentTextChanged(QString)), this, SLOT(onChangePlatformB(QString)));
    QObject::connect(ui->m_pactionAddPlatform, SIGNAL(triggered(bool)), this, SLOT(onAddPlatformActionTriggered(bool)));
    QObject::connect(ui->m_pbtnAttachLoginInfoA, SIGNAL(clicked()), this, SLOT(onAttachButtonAClicked()));
    QObject::connect(ui->m_pbtnAttachLoginInfoB, SIGNAL(clicked()), this, SLOT(onAttachButtonBClicked()));

    QObject::connect(ui->m_pbtnEditLoginInfoA, SIGNAL(clicked()), this, SLOT(onEditLoginInfoAButtonClicked()));
    QObject::connect(ui->m_pbtnEditLoginInfoB, SIGNAL(clicked()), this, SLOT(onEditLoginInfoBButtonClicked()));

    QObject::connect(ui->m_pspinBuyAmountFromA, SIGNAL(valueChanged(double)), this, SLOT(onChangeAmountBTCA(double)));
    QObject::connect(ui->m_pspinBuyAmountFromB, SIGNAL(valueChanged(double)), this, SLOT(onChangeAmountBTCB(double)));

    QObject::connect(ui->m_pbtnOrderAtoB, SIGNAL(clicked()), this, SLOT(onOrderButtonAClicked()));
    QObject::connect(ui->m_pbtnOrderBtoA, SIGNAL(clicked()), this, SLOT(onOrderButtonBClicked()));

    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    onChangePlatformA("Binance");
    onChangePlatformB("HITBTC");

    m_dfMinProfitSum = 0.0;
    m_dfMaxProfitSum = 0.0;
    m_nTradeCounter = 1;

    QStringList strHeaders;
    strHeaders << "Buy Platform" << "Amount" << "Price" << "Fee" << "Sell Platform" << "Amount" << "Price" << "Fee" << "Min Profit" << "Max Profit";
    ui->m_ptblTradeHistory->setHorizontalHeaderLabels(strHeaders);

    QString strLogFileName = QDateTime::currentDateTime().toString(Qt::ISODate);
    strLogFileName.replace(":", "-");
    QString strTradingLogFileName = strLogFileName + "-trading.log";
    QString strOrderingLogFileName = strLogFileName + "-ordering.log";


    m_pCompletedTradingLogger = new CLogger(strTradingLogFileName);
    m_pCompletedOrderingLogger = new CLogger(strOrderingLogFileName);

    m_Timer.setInterval(1000);
    m_Timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_pCompletedTradingLogger) {
        delete m_pCompletedTradingLogger;
    }

    if (m_pCompletedOrderingLogger) {
        delete m_pCompletedOrderingLogger;
    }
}

QDockWidget *MainWindow::createDock(QWidget *pWidget, const QString &strTitle)
{
    pWidget->setProperty("IsDockable", true);
    QDockWidget *pDockWidget = m_pDockHost->createDock(this, pWidget, strTitle);
    return pDockWidget;
}

void MainWindow::initDocks()
{
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
}

void MainWindow::moveWidgetsToDocks()
{
    // left
    QDockWidget *pdockPlatformASelection = createDock(ui->m_pPlatformASelection, tr("Platform A"));
    QDockWidget *pdockPlatformBSelection = createDock(ui->m_pPlatformBSelection, tr("Platform B"));

    addDockWidget(Qt::LeftDockWidgetArea, pdockPlatformASelection);
    splitDockWidget(pdockPlatformASelection, pdockPlatformBSelection, Qt::Vertical);

    // right
    QDockWidget *pdockPlatformMarkets = createDock(ui->m_pPlatformMarkets, tr("Platform Markets"));
    addDockWidget(Qt::RightDockWidgetArea, pdockPlatformMarkets);

    // Central
    QDockWidget* centralDockNULL = new QDockWidget(this);
    centralDockNULL->setFixedWidth(0);
    setCentralWidget(centralDockNULL);
}

CExchange * MainWindow::createExchangeFromName(QString strName)
{
    QSettings platformSettings("platforms.ini", QSettings::IniFormat, this);
    platformSettings.beginGroup(strName);
    QString strClassName = platformSettings.value("classname").toString();

    if (strClassName.isEmpty()) {
        return nullptr;
    }

    const QMetaObject *pMetaObject = m_metaObjectLookup[strClassName];
    if (pMetaObject == nullptr) {
        return nullptr;
    }

    CExchange *pExchange = static_cast<CExchange *>(pMetaObject->newInstance());

    QUrl APIURL = platformSettings.value("url").toUrl();
    QByteArray btarrayAPIKey = platformSettings.value("apikey").toByteArray();
    QByteArray btarraySecKey = platformSettings.value("seckey").toByteArray();

    pExchange->setPlatformName(strName);
    pExchange->setAPIURL(APIURL);
    pExchange->setAPIKey(btarrayAPIKey);
    pExchange->setSecKey(btarraySecKey);

    return pExchange;
}

// slots
void MainWindow::onChangePlatformA(QString strPlatformName)
{
    QString strPlatformBName = ui->m_pPlatformsListB->currentText();

    if (strPlatformBName == strPlatformName) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not set the PlatformA name same with PlatformB"));
        ui->m_pPlatformsListA->setCurrentText(m_strPlatformA);
        return;
    }

    m_strPlatformA = strPlatformName;

    QSettings platformSettings("platforms.ini", QSettings::IniFormat, this);
    platformSettings.beginGroup(strPlatformName);
    QByteArray btarrayAPIKey = platformSettings.value("apikey").toByteArray();
    QByteArray btarraySecKey = platformSettings.value("seckey").toByteArray();
    ui->m_ptxtAPIKeyA->setText(btarrayAPIKey);
    ui->m_ptxtSecKeyA->setText(btarraySecKey);
}

void MainWindow::onChangePlatformB(QString strPlatformName)
{
    QString strPlatformAName = ui->m_pPlatformsListA->currentText();
    if (strPlatformAName == strPlatformName) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not set the PlatformB name same with PlatformA"));
        ui->m_pPlatformsListB->setCurrentText(m_strPlatformB);
        return;
    }

    m_strPlatformB = strPlatformName;

    QSettings platformSettings("platforms.ini", QSettings::IniFormat, this);
    platformSettings.beginGroup(strPlatformName);
    QByteArray btarrayAPIKey = platformSettings.value("apikey").toByteArray();
    QByteArray btarraySecKey = platformSettings.value("seckey").toByteArray();
    ui->m_ptxtAPIKeyB->setText(btarrayAPIKey);
    ui->m_ptxtSecKeyB->setText(btarraySecKey);
}

void MainWindow::onAddPlatformActionTriggered(bool)
{
    CAddPlatformDialog addPlatformDlg(this);
    if (addPlatformDlg.exec() == QDialog::Rejected) {
        return;
    }

    QString strPlatform = addPlatformDlg.getPlatformName();
    QString strClassName = addPlatformDlg.getClassName();
    QUrl apiURL = addPlatformDlg.getAPIURL();
    QByteArray btarrayAPIKey = addPlatformDlg.getAPIKey();
    QByteArray btarrayAPISec = addPlatformDlg.getSecKey();

    QSettings platformSettings("platforms.ini", QSettings::IniFormat, this);
    platformSettings.beginGroup(strPlatform);
    platformSettings.setValue("classname", strClassName);
    platformSettings.setValue("url", apiURL);
    platformSettings.setValue("apikey", btarrayAPIKey);
    platformSettings.setValue("seckey", btarrayAPISec);
    platformSettings.endGroup();
}

void MainWindow::onAttachButtonAClicked()
{
    QString strPlatformA = ui->m_pPlatformsListA->currentText();
    CExchange *pExchange = createExchangeFromName(strPlatformA);

    if (pExchange == nullptr) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not create Exchange object for %1 platform").arg(strPlatformA));
        return;
    }

    m_pExchangeA = pExchange;
    if (strPlatformA != "Binance")
        m_pExchangeA->readFee();
    m_pExchangeA->readBalance();
    QObject::connect(m_pExchangeA, SIGNAL(receivedBalance(double, double)), this, SLOT(onReceivedPlatformABalance(double, double)));
    QObject::connect(m_pExchangeA, SIGNAL(receivedTradingFee(double)), this, SLOT(onReceivedFeeA(double)));
    //QObject::connect(m_pExchangeA, SIGNAL(receivedTicker(CExchange::Ticker)), this, SLOT(onReceivedPlatformATicker(CExchange::Ticker)));
}

void MainWindow::onAttachButtonBClicked()
{
    QString strPlatformB = ui->m_pPlatformsListB->currentText();
    CExchange *pExchange = createExchangeFromName(strPlatformB);

    if (pExchange == nullptr) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not create Exchange object for %1 platform").arg(strPlatformB));
        return;
    }

    m_pExchangeB = pExchange;
    if (strPlatformB != "Binance")
        m_pExchangeB->readFee();

    m_pExchangeB->readBalance();
    QObject::connect(m_pExchangeB, SIGNAL(receivedBalance(double, double)), this, SLOT(onReceivedPlatformBBalance(double, double)));
    QObject::connect(m_pExchangeB, SIGNAL(receivedTradingFee(double)), this, SLOT(onReceivedFeeB(double)));
    //QObject::connect(m_pExchangeB, SIGNAL(receivedTicker(CExchange::Ticker)), this, SLOT(onReceivedPlatformBTicker(CExchange::Ticker)));
}

void MainWindow::onEditLoginInfoAButtonClicked()
{
    bool isAPIKeyEnabled = ui->m_ptxtAPIKeyA->isEnabled();
    bool isSecKeyEnabled = ui->m_ptxtSecKeyA->isEnabled();
    ui->m_ptxtAPIKeyA->setEnabled(!isAPIKeyEnabled);
    ui->m_ptxtSecKeyA->setEnabled(!isSecKeyEnabled);
    if (!isAPIKeyEnabled && !isSecKeyEnabled){
        ui->m_pbtnEditLoginInfoA->setText(tr("Edited"));
    }
    else {
        ui->m_pbtnEditLoginInfoA->setText(tr("Edit"));
        QSettings platformSettings("platforms.ini", QSettings::IniFormat, this);
        platformSettings.beginGroup(ui->m_pPlatformsListA->currentText());
        QString strAPIKey = ui->m_ptxtAPIKeyA->text().remove(QRegExp("[^a-zA-Z0-9-+/=\\d]"));
        QString strSecKey = ui->m_ptxtSecKeyA->text().remove(QRegExp("[^a-zA-Z0-9-+/=\\d]"));
        platformSettings.setValue("apikey", strAPIKey.toLatin1());
        platformSettings.setValue("seckey", strSecKey.toLatin1());
    }
}

void MainWindow::onEditLoginInfoBButtonClicked()
{
    bool isAPIKeyEnabled = ui->m_ptxtAPIKeyB->isEnabled();
    bool isSecKeyEnabled = ui->m_ptxtSecKeyB->isEnabled();
    ui->m_ptxtAPIKeyB->setEnabled(!isAPIKeyEnabled);
    ui->m_ptxtSecKeyB->setEnabled(!isSecKeyEnabled);
    if (!isAPIKeyEnabled && !isSecKeyEnabled){
        ui->m_pbtnEditLoginInfoB->setText(tr("Edited"));
    }
    else {
        ui->m_pbtnEditLoginInfoB->setText(tr("Edit"));
        QSettings platformSettings("platforms.ini", QSettings::IniFormat, this);
        platformSettings.beginGroup(ui->m_pPlatformsListB->currentText());
        QString strAPIKey = ui->m_ptxtAPIKeyB->text().remove(QRegExp("[^a-zA-Z0-9-+/=\\d]"));
        QString strSecKey = ui->m_ptxtSecKeyB->text().remove(QRegExp("[^a-zA-Z0-9-+/=\\d]"));
        platformSettings.setValue("apikey", strAPIKey.toLatin1());
        platformSettings.setValue("seckey", strSecKey.toLatin1());
    }
}

void MainWindow::onChangeAmountBTCA(double dfValue)
{
    ui->m_pspinSellAmountToB->setValue(dfValue);
}

void MainWindow::onChangeAmountBTCB(double dfValue)
{
    ui->m_pspinSellAmountToA->setValue(dfValue);
}

void MainWindow::onOrderButtonAClicked()
{
    startTradeAtoB();
}

void MainWindow::onOrderButtonBClicked()
{
    startTradeBtoA();
}

void MainWindow::onReceivedPlatformABalance(double dfValue1, double dfValue2)
{
    qDebug() << dfValue1 << dfValue2;
    ui->m_pspinCur1BalanceA->setValue(dfValue1);
    ui->m_pspinCur2BalanceA->setValue(dfValue2);
    m_isReceivedPlatformABalance = true;
}

void MainWindow::onReceivedFeeA(double dfFee)
{
    ui->m_pspinTradingFeeA->setValue(dfFee);
}

void MainWindow::onReceivedFeeB(double dfFee)
{
    ui->m_pspinTradingFeeB->setValue(dfFee);
}

void MainWindow::onReceivedPlatformBBalance(double dfValue1, double dfValue2)
{
    qDebug() << dfValue1 << dfValue2;
    ui->m_pspinCur1BalanceB->setValue(dfValue1); // BTC
    ui->m_pspinCur2BalanceB->setValue(dfValue2); // USD
    m_isReceivedPlatformBBalance = true;
}

void MainWindow::onSucceedOrdering(CExchange *pExchange, int nID, CExchange::OrderType orderType)
{
    pExchange->readBalance();
    TradeHistory record = m_TradeResults.value(nID);

    QString strCSVRecord = makeCSVRecord(record);
    if (orderType == CExchange::BUY)
        strCSVRecord = "BUY," + strCSVRecord;
    else if(orderType == CExchange::SELL)
        strCSVRecord = "SELL," + strCSVRecord;

    m_pCompletedOrderingLogger->writeLog(strCSVRecord);

    int nCompletions = record.nCompletions;
    ++nCompletions;
    if (nCompletions >= 2) {
        logWithCSVFormat(m_pCompletedTradingLogger, record);
        addTradeHistoryRecord(record);
        m_TradeResults.remove(nID);
        qDebug() << QString("Trade %1 completed").arg(nID);
    } else {
        record.nCompletions = nCompletions;
        m_TradeResults.insert(nID, record);
    }
}

void MainWindow::addTradeHistoryRecord(TradeHistory record)
{
    QTableWidgetItem *pBuyPlatform = new QTableWidgetItem(record.strBuyPlatform);
    QTableWidgetItem *pBuyAmount = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(record.dfBuyAmount)));
    QTableWidgetItem *pBuyPrice = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(record.dfBuyPrice)));
    QTableWidgetItem *pBuyFee = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(record.dfBuyFee)));
    QTableWidgetItem *pSellPlatform = new QTableWidgetItem(record.strSellPlatform);
    QTableWidgetItem *pSellAmount = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(record.dfSellAmount)));
    QTableWidgetItem *pSellPrice = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(record.dfSellPrice)));
    QTableWidgetItem *pSellFee = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(record.dfSellFee)));

    double dfMinProfit, dfMaxProfit;
    calculateProfit(record, &dfMinProfit, &dfMaxProfit);
    QTableWidgetItem *pMinProfit = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(dfMinProfit)));
    QTableWidgetItem *pMaxProfit = new QTableWidgetItem(QString(CExchange::byteArrayFromDouble(dfMaxProfit)));

    m_dfMinProfitSum += dfMinProfit;
    m_dfMaxProfitSum += dfMaxProfit;

    ui->m_plblTotalSum->setText(QString("Total(Min Profit = %1, Max Profit = %2)").arg(QString(CExchange::byteArrayFromDouble(m_dfMinProfitSum))).arg(QString(CExchange::byteArrayFromDouble(m_dfMaxProfitSum))));

    int nRowCount = ui->m_ptblTradeHistory->rowCount();
    ui->m_ptblTradeHistory->setRowCount(nRowCount + 1);
    ui->m_ptblTradeHistory->setItem(nRowCount, 0, pBuyPlatform);
    ui->m_ptblTradeHistory->setItem(nRowCount, 1, pBuyAmount);
    ui->m_ptblTradeHistory->setItem(nRowCount, 2, pBuyPrice);
    ui->m_ptblTradeHistory->setItem(nRowCount, 3, pBuyFee);
    ui->m_ptblTradeHistory->setItem(nRowCount, 4, pSellPlatform);
    ui->m_ptblTradeHistory->setItem(nRowCount, 5, pSellAmount);
    ui->m_ptblTradeHistory->setItem(nRowCount, 6, pSellPrice);
    ui->m_ptblTradeHistory->setItem(nRowCount, 7, pSellFee);
    ui->m_ptblTradeHistory->setItem(nRowCount, 8, pMinProfit);
    ui->m_ptblTradeHistory->setItem(nRowCount, 9, pMaxProfit);
}

void MainWindow::calculateProfit(TradeHistory record, double *pdfMinProfit, double *pdfMaxProfit)
{
    double dfBuy = record.dfBuyPrice * record.dfBuyAmount;
    double dfSell = record.dfSellPrice * record.dfSellAmount;
    double dfBuyFee = record.dfBuyFee * dfBuy;
    double dfSellFee = record.dfSellFee * dfSell;
    double dfMaxProfit = dfSell - dfBuy;
    double dfMinProfit = dfMaxProfit - (dfBuyFee + dfSellFee) / 100;
    *pdfMinProfit = dfMinProfit;
    *pdfMaxProfit = dfMaxProfit;
}

void MainWindow::onErrorBuyingBTCfromA(int, QString, int nID)
{
    m_TradeResults.remove(nID);
}

void MainWindow::onErrorSellingBTCtoA(int, QString, int nID)
{
    m_TradeResults.remove(nID);
}

void MainWindow::onErrorBuyingBTCfromB(int, QString, int nID)
{
    m_TradeResults.remove(nID);
}

void MainWindow::onErrorSellingBTCtoB(int, QString, int nID)
{
    m_TradeResults.remove(nID);
}

void MainWindow::onErrorOrder(int)
{

}

void MainWindow::updateView()
{
    double dfBuyPriceA = m_pExchangeA->getBuyPrice();
    double dfSellPriceA = m_pExchangeA->getSellPrice();
    double dfBuyPriceB = m_pExchangeB->getBuyPrice();
    double dfSellPriceB = m_pExchangeB->getSellPrice();

    // update platformA's view
    ui->m_pspinBuyPriceA->setValue(dfBuyPriceA);
    ui->m_pspinSellPriceA->setValue(dfSellPriceA);
    ui->m_pspinHighPriceA->setValue(m_pExchangeA->getHighPrice());
    ui->m_pspinLowPriceA->setValue(m_pExchangeA->getLowPrice());
    ui->m_pspinLastTradePriceA->setValue(m_pExchangeA->getLastPrice());
    ui->m_pspinVolumeA->setValue(m_pExchangeA->getVolume());

    // update platformB's view
    ui->m_pspinBuyPriceB->setValue(dfBuyPriceB);
    ui->m_pspinSellPriceB->setValue(dfSellPriceB);
    ui->m_pspinHighPriceB->setValue(m_pExchangeB->getHighPrice());
    ui->m_pspinLowPriceB->setValue(m_pExchangeB->getLowPrice());
    ui->m_pspinLastTradePriceB->setValue(m_pExchangeB->getLastPrice());
    ui->m_pspinVolumeB->setValue(m_pExchangeB->getVolume());

    // Trading's view
    double dfDifferenceFromAtoB = dfSellPriceB - dfBuyPriceA;
    double dfDifferenceFromBtoA = dfSellPriceA - dfBuyPriceB;
    double dfPercentA = dfDifferenceFromAtoB / dfBuyPriceA * 100;
    double dfPercentB = dfDifferenceFromBtoA / dfBuyPriceB * 100;

    ui->m_pspinDifferFromAtoB->setValue(dfDifferenceFromAtoB);
    ui->m_pspinDifferFromBtoA->setValue(dfDifferenceFromBtoA);
    ui->m_pspinDiffPercentA->setValue(dfPercentA);
    ui->m_pspinDiffPercentB->setValue(dfPercentB);


    double dfBuyAmountA = ui->m_pspinBuyAmountFromA->value();
    double dfSellAmountB = ui->m_pspinSellAmountToB->value();
    double dfFeeAtoB1 = m_pExchangeA->getFee() * (dfBuyPriceA * dfBuyAmountA);
    double dfFeeAtoB2 = m_pExchangeB->getFee() * (dfSellPriceB * dfSellAmountB);
    double dfFeeAB = (dfFeeAtoB1 + dfFeeAtoB2) / 100;
    double dfProfitAtoB = dfBuyAmountA * dfDifferenceFromAtoB - dfFeeAB;

    double dfBuyAmountB = ui->m_pspinBuyAmountFromB->value();
    double dfSellAmountA = ui->m_pspinSellAmountToA->value();
    double dfFeeBtoA1 = m_pExchangeB->getFee() * (dfBuyPriceB * dfBuyAmountB);
    double dfFeeBtoA2 = m_pExchangeA->getFee() * (dfSellPriceA * dfSellAmountA);
    double dfFeeBA = (dfFeeBtoA1 + dfFeeBtoA2) / 100;
    double dfProfitBtoA = dfBuyAmountB * dfDifferenceFromBtoA - dfFeeBA;

    ui->m_pspinProfitA->setValue(dfProfitAtoB);
    ui->m_pspinProfitB->setValue(dfProfitBtoA);

    double dfStartRateAtoB = ui->m_pspinStartRateAtoB->value();
    if(dfProfitAtoB > (dfBuyPriceA * dfBuyAmountA * dfStartRateAtoB)) {
    //if (dfStartRateAtoB * dfBuyPriceA < dfSellPriceB) {
        ui->m_pbtnOrderAtoB->setEnabled(true);
    } else {
        ui->m_pbtnOrderAtoB->setEnabled(false);
    }

    double dfStartRateBtoA = ui->m_pspinStartRateBtoA->value();
    if (dfProfitBtoA > (dfBuyPriceB * dfBuyAmountB * dfStartRateBtoA)) {
    //if (dfStartRateBtoA * dfBuyPriceB < dfSellPriceA) {
        ui->m_pbtnOrderBtoA->setEnabled(true);
    } else {
        ui->m_pbtnOrderBtoA->setEnabled(false);
    }
}

void MainWindow::onTimer()
{
    if (m_isReceivedPlatformABalance)
        m_pExchangeA->readTicker();

    if (m_isReceivedPlatformBBalance)
        m_pExchangeB->readTicker();

    if (m_isReceivedPlatformABalance && m_isReceivedPlatformBBalance)
        updateView();
}

void MainWindow::startTradeAtoB()
{
    double dfBuyAmountFromA = ui->m_pspinBuyAmountFromA->value();
    double dfBuyPriceFromA = ui->m_pspinBuyPriceA->value();
    double dfSellAmountB = ui->m_pspinSellAmountToB->value();
    double dfSellPriceToB = ui->m_pspinSellPriceB->value();

    if (dfBuyAmountFromA == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Input amount of BTC to buy"));
        return;
    }

    if (dfBuyPriceFromA == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not buy BTC with 0$"));
        return;
    }

    if (dfSellAmountB == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Input amount of BTC to sell"));
        return;
    }

    if (dfSellPriceToB == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not sell BTC with 0$"));
        return;
    }

    double dfUSDBalance = m_pExchangeA->getCur2Balance();
    if (dfBuyAmountFromA * dfBuyPriceFromA > dfUSDBalance) {
        QMessageBox::warning(this, tr("Warning"), tr("Insufficient USD balance to buy BTC from PlatformA"));
        return;
    }

    double dfBTCBalance = m_pExchangeB->getCur1Balance();
    if (dfSellAmountB > dfBTCBalance) {
        QMessageBox::warning(this, tr("Warning"), tr("Smaller BTC balance than request amount on platformB"));
        return;
    }

    m_pExchangeA->buy("BTCUSDT", dfBuyAmountFromA, dfBuyPriceFromA, m_nTradeCounter);
    m_pExchangeB->sell("BTCUSD", dfSellAmountB, dfSellPriceToB, m_nTradeCounter);

    TradeHistory record;
    record.nID = m_nTradeCounter;
    record.dfBuyPrice = dfBuyPriceFromA;
    record.dfBuyAmount = dfBuyAmountFromA;
    record.dfSellPrice = dfSellPriceToB;
    record.dfSellAmount = dfSellAmountB;
    record.dfBuyFee = m_pExchangeA->getFee();
    record.dfSellFee = m_pExchangeB->getFee();
    record.strBuyPlatform = m_pExchangeA->getPlatformName();
    record.strSellPlatform = m_pExchangeB->getPlatformName();
    record.nCompletions = 0;

    m_TradeResults.insert(m_nTradeCounter, record);

    m_nTradeCounter++;

    QObject::connect(m_pExchangeA, SIGNAL(succeedOrderingBTC(CExchange *, int, CExchange::OrderType)), this, SLOT(onSucceedOrdering(CExchange *, int, CExchange::OrderType)));
    QObject::connect(m_pExchangeB, SIGNAL(succeedOrderingBTC(CExchange *, int, CExchange::OrderType)), this, SLOT(onSucceedOrdering(CExchange *, int, CExchange::OrderType)));
}

void MainWindow::startTradeBtoA()
{
    double dfBuyAmountFromB = ui->m_pspinBuyAmountFromB->value();
    double dfBuyPriceFromB = ui->m_pspinBuyPriceB->value();
    double dfSellAmountToA = ui->m_pspinSellAmountToA->value();
    double dfSellPriceToA = ui->m_pspinSellPriceA->value();

    if (dfBuyAmountFromB == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Input amount BTC to buy"));
        return;
    }

    if (dfBuyPriceFromB == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not buy BTC with 0$"));
        return;
    }

    if (dfSellAmountToA == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Input amount BTC to sell"));
        return;
    }

    if (dfSellPriceToA == 0.0) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not sell BTC with 0$"));
        return;
    }

    double dfUSDBalance = m_pExchangeB->getCur2Balance();
    if (dfBuyAmountFromB * dfBuyPriceFromB > dfUSDBalance) {
        QMessageBox::warning(this, tr("Warning"), tr("Insufficient USD balance to buy BTC from PlatformB"));
        return;
    }

    double dfBTCBalance = m_pExchangeA->getCur1Balance();
    if (dfSellAmountToA > dfBTCBalance) {
        QMessageBox::warning(this, tr("Warning"), tr("Smaller BTC balance than request amount on platformA"));
        return;
    }

    m_pExchangeB->buy("BTCUSD", dfBuyAmountFromB, dfBuyPriceFromB, m_nTradeCounter);
    m_pExchangeA->sell("BTCUSD", dfSellAmountToA, dfSellPriceToA, m_nTradeCounter);

    TradeHistory record;
    record.nID = m_nTradeCounter;
    record.dfBuyPrice = dfBuyPriceFromB;
    record.dfBuyAmount = dfBuyAmountFromB;
    record.dfSellPrice = dfSellPriceToA;
    record.dfSellAmount = dfSellAmountToA;
    record.dfBuyFee = m_pExchangeB->getFee();
    record.dfSellFee = m_pExchangeA->getFee();
    record.strBuyPlatform = m_pExchangeB->getPlatformName();
    record.strSellPlatform = m_pExchangeA->getPlatformName();
    record.nCompletions = 0;

    m_TradeResults.insert(m_nTradeCounter, record);

    m_nTradeCounter++;

    QObject::connect(m_pExchangeB, SIGNAL(succeedOrderingBTC(CExchange *, int, CExchange::OrderType)), this, SLOT(onSucceedOrdering(CExchange *, int, CExchange::OrderType)));
    QObject::connect(m_pExchangeA, SIGNAL(succeedOrderingBTC(CExchange *, int, CExchange::OrderType)), this, SLOT(onSucceedOrdering(CExchange *, int, CExchange::OrderType)));
}

QString MainWindow::makeCSVRecord(TradeHistory record)
{
    QString separator = ",";
    QString strRecord = QString::number(record.nID);
    strRecord += separator + record.strBuyPlatform;
    strRecord += separator + CExchange::byteArrayFromDouble(record.dfBuyPrice);
    strRecord += separator + CExchange::byteArrayFromDouble(record.dfBuyAmount);
    strRecord += separator + CExchange::byteArrayFromDouble(record.dfBuyFee);
    strRecord += separator + record.strSellPlatform;
    strRecord += separator + CExchange::byteArrayFromDouble(record.dfSellPrice);
    strRecord += separator + CExchange::byteArrayFromDouble(record.dfSellAmount);
    strRecord += separator + CExchange::byteArrayFromDouble(record.dfSellFee);
    return strRecord;
}

void MainWindow::logWithCSVFormat(CLogger *pLogger, TradeHistory record)
{
    pLogger->writeLog(makeCSVRecord(record));
}
