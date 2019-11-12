/********************************************************************************
** Form generated from reading UI file 'CAddPlatformDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CADDPLATFORMDIALOG_H
#define UI_CADDPLATFORMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_CAddPlatformDialog
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLineEdit *m_ptxtAPIURL;
    QLabel *label_3;
    QLineEdit *m_ptxtSecKey;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *m_ptxtAPIKey;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_pbtnOK;
    QPushButton *m_pbtnCancel;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QLineEdit *m_ptxtPlatformName;
    QLabel *label_5;
    QLineEdit *m_ptxtClassName;

    void setupUi(QDialog *CAddPlatformDialog)
    {
        if (CAddPlatformDialog->objectName().isEmpty())
            CAddPlatformDialog->setObjectName(QStringLiteral("CAddPlatformDialog"));
        CAddPlatformDialog->resize(473, 149);
        gridLayout_2 = new QGridLayout(CAddPlatformDialog);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        m_ptxtAPIURL = new QLineEdit(CAddPlatformDialog);
        m_ptxtAPIURL->setObjectName(QStringLiteral("m_ptxtAPIURL"));

        gridLayout->addWidget(m_ptxtAPIURL, 1, 1, 1, 1);

        label_3 = new QLabel(CAddPlatformDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        m_ptxtSecKey = new QLineEdit(CAddPlatformDialog);
        m_ptxtSecKey->setObjectName(QStringLiteral("m_ptxtSecKey"));

        gridLayout->addWidget(m_ptxtSecKey, 3, 1, 1, 1);

        label_2 = new QLabel(CAddPlatformDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        label = new QLabel(CAddPlatformDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        m_ptxtAPIKey = new QLineEdit(CAddPlatformDialog);
        m_ptxtAPIKey->setObjectName(QStringLiteral("m_ptxtAPIKey"));

        gridLayout->addWidget(m_ptxtAPIKey, 2, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        m_pbtnOK = new QPushButton(CAddPlatformDialog);
        m_pbtnOK->setObjectName(QStringLiteral("m_pbtnOK"));

        horizontalLayout->addWidget(m_pbtnOK);

        m_pbtnCancel = new QPushButton(CAddPlatformDialog);
        m_pbtnCancel->setObjectName(QStringLiteral("m_pbtnCancel"));

        horizontalLayout->addWidget(m_pbtnCancel);


        gridLayout->addLayout(horizontalLayout, 4, 0, 1, 2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_4 = new QLabel(CAddPlatformDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_2->addWidget(label_4);

        m_ptxtPlatformName = new QLineEdit(CAddPlatformDialog);
        m_ptxtPlatformName->setObjectName(QStringLiteral("m_ptxtPlatformName"));

        horizontalLayout_2->addWidget(m_ptxtPlatformName);

        label_5 = new QLabel(CAddPlatformDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_2->addWidget(label_5);

        m_ptxtClassName = new QLineEdit(CAddPlatformDialog);
        m_ptxtClassName->setObjectName(QStringLiteral("m_ptxtClassName"));

        horizontalLayout_2->addWidget(m_ptxtClassName);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 2);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        QWidget::setTabOrder(m_ptxtPlatformName, m_ptxtClassName);
        QWidget::setTabOrder(m_ptxtClassName, m_ptxtAPIURL);
        QWidget::setTabOrder(m_ptxtAPIURL, m_ptxtAPIKey);
        QWidget::setTabOrder(m_ptxtAPIKey, m_ptxtSecKey);
        QWidget::setTabOrder(m_ptxtSecKey, m_pbtnOK);
        QWidget::setTabOrder(m_pbtnOK, m_pbtnCancel);

        retranslateUi(CAddPlatformDialog);

        QMetaObject::connectSlotsByName(CAddPlatformDialog);
    } // setupUi

    void retranslateUi(QDialog *CAddPlatformDialog)
    {
        CAddPlatformDialog->setWindowTitle(QApplication::translate("CAddPlatformDialog", "Dialog", Q_NULLPTR));
        label_3->setText(QApplication::translate("CAddPlatformDialog", "API URL", Q_NULLPTR));
        label_2->setText(QApplication::translate("CAddPlatformDialog", "Sec Key", Q_NULLPTR));
        label->setText(QApplication::translate("CAddPlatformDialog", "API Key", Q_NULLPTR));
        m_pbtnOK->setText(QApplication::translate("CAddPlatformDialog", "OK", Q_NULLPTR));
        m_pbtnCancel->setText(QApplication::translate("CAddPlatformDialog", "Cancel", Q_NULLPTR));
        label_4->setText(QApplication::translate("CAddPlatformDialog", "Platform", Q_NULLPTR));
        label_5->setText(QApplication::translate("CAddPlatformDialog", "Class Name", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CAddPlatformDialog: public Ui_CAddPlatformDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CADDPLATFORMDIALOG_H
