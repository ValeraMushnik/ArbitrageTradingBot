#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class DockHost;
#include "CExchange.h"
#include "CLogger.h"

typedef struct {
    QString strBuyPlatform;
    QString strSellPlatform;
    double dfBuyPrice;
    double dfSellPrice;
    double dfBuyFee;
    double dfSellFee;
    double dfBuyAmount;
    double dfSellAmount;
    int    nCompletions;
    int    nID;
}TradeHistory;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    DockHost *                              m_pDockHost;

    CExchange *                             m_pExchangeA;
    CExchange *                             m_pExchangeB;
    bool                                    m_isReceivedPlatformABalance;
    bool                                    m_isReceivedPlatformBBalance;

    QString                                 m_strPlatformA;
    QString                                 m_strPlatformB;

    QMap<QString, const QMetaObject*>       m_metaObjectLookup;

    QMap<int, TradeHistory>                 m_TradeResults;
    double                                  m_dfMinProfitSum;
    double                                  m_dfMaxProfitSum;
    int                                     m_nTradeCounter;
    QTimer                                  m_Timer;

    CLogger                                 *m_pCompletedTradingLogger;
    CLogger                                 *m_pCompletedOrderingLogger;

public:
    explicit                                MainWindow(QWidget *parent = nullptr);
                                            ~MainWindow();

    QDockWidget *                           createDock(QWidget *pWidget, const QString &strTitle);
    void                                    initDocks();
    void                                    moveWidgetsToDocks();
    CExchange *                             createExchangeFromName(QString strName);
    void                                    calculateProfit(TradeHistory record, double *dfMinProfit, double *dfMaxProfit);

public slots:

    void                                    onChangePlatformA(QString);
    void                                    onChangePlatformB(QString);
    void                                    onAddPlatformActionTriggered(bool isChecked = false);
    void                                    onAttachButtonAClicked();
    void                                    onAttachButtonBClicked();
    void                                    onEditLoginInfoAButtonClicked();
    void                                    onEditLoginInfoBButtonClicked();
    void                                    onChangeAmountBTCA(double);
    void                                    onChangeAmountBTCB(double);
    void                                    onOrderButtonAClicked();
    void                                    onOrderButtonBClicked();
    void                                    onReceivedFeeA(double);
    void                                    onReceivedFeeB(double);
    void                                    onReceivedPlatformABalance(double, double);
    void                                    onReceivedPlatformBBalance(double, double);

    void                                    onSucceedOrdering(CExchange *pExchange, int nID, CExchange::OrderType orderType);

    void                                    onErrorBuyingBTCfromA(int, QString, int);
    void                                    onErrorSellingBTCtoA(int, QString, int);
    void                                    onErrorBuyingBTCfromB(int, QString, int);
    void                                    onErrorSellingBTCtoB(int, QString, int);

    void                                    addTradeHistoryRecord(TradeHistory record);
    void                                    onErrorOrder(int nID);

    void                                    startTradeAtoB();
    void                                    startTradeBtoA();

    void                                    updateView();

    void                                    onTimer();
    QString                                 makeCSVRecord(TradeHistory record);
    void                                    logWithCSVFormat(CLogger *, TradeHistory);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
