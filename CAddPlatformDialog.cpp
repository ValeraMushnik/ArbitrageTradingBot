#include "CAddPlatformDialog.h"
#include "ui_CAddPlatformDialog.h"
#include <QMessageBox>

CAddPlatformDialog::CAddPlatformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAddPlatformDialog)
{
    ui->setupUi(this);
    //this->setFixedSize(this->size());
    QObject::connect(ui->m_pbtnOK, SIGNAL(clicked()), this, SLOT(onOKButtonClicked()));
    QObject::connect(ui->m_pbtnCancel, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
}

CAddPlatformDialog::~CAddPlatformDialog()
{
    delete ui;
}

QString CAddPlatformDialog::getPlatformName()
{
    return m_strPlatformName;
}

QString CAddPlatformDialog::getClassName()
{
    return m_strClassName;
}

QUrl CAddPlatformDialog::getAPIURL()
{
    return m_APIURL;
}

QByteArray CAddPlatformDialog::getAPIKey()
{
    return m_btarrayAPIKey;
}

QByteArray CAddPlatformDialog::getSecKey()
{
    return m_btarraySecKey;
}

//slots
void CAddPlatformDialog::onOKButtonClicked()
{
    QString strPlatformName = ui->m_ptxtPlatformName->text();
    QString strClassName = ui->m_ptxtClassName->text();
    QString strAPIURL = ui->m_ptxtAPIURL->text();
    QString strAPIKey = ui->m_ptxtAPIKey->text();
    QString strSecKey = ui->m_ptxtSecKey->text();

    strClassName = strClassName.remove(QRegExp("[^a-zA-Z0-9-+/=\\d]"));
    strAPIKey = strAPIKey.remove(QRegExp("[^a-zA-Z0-9-+/=\\d]"));
    strSecKey = strSecKey.remove(QRegExp("[^a-zA-Z0-9-+/=\\d]"));

    if (strAPIKey.isEmpty() || strSecKey.isEmpty() || strAPIURL.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Login Info could not be contain empty value"));
        return;
    }

    m_strPlatformName = strPlatformName;
    m_strClassName = strClassName;

    m_APIURL = QUrl(strAPIURL);
    m_btarrayAPIKey = strAPIKey.toLatin1();
    m_btarraySecKey = strSecKey.toLatin1();

    this->done(QDialog::Accepted);
}

void CAddPlatformDialog::onCancelButtonClicked()
{
    this->done(QDialog::Rejected);
}
