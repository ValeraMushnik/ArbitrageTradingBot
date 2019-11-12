#ifndef CADDPLATFORMDIALOG_H
#define CADDPLATFORMDIALOG_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class CAddPlatformDialog;
}

class CAddPlatformDialog : public QDialog
{
    Q_OBJECT

private:

    QString                                 m_strPlatformName;
    QString                                 m_strClassName;
    QUrl                                    m_APIURL;
    QByteArray                              m_btarrayAPIKey;
    QByteArray                              m_btarraySecKey;

public:
    explicit                                CAddPlatformDialog(QWidget *parent = nullptr);
                                            ~CAddPlatformDialog();

    QString                                 getPlatformName();
    QString                                 getClassName();
    QUrl                                    getAPIURL();
    QByteArray                              getAPIKey();
    QByteArray                              getSecKey();

public slots:
    void                                    onOKButtonClicked();
    void                                    onCancelButtonClicked();

private:
    Ui::CAddPlatformDialog *ui;
};

#endif // CADDPLATFORMDIALOG_H
