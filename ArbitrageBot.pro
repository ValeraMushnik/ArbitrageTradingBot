#-------------------------------------------------
#
# Project created by QtCreator 2019-07-14T07:37:27
#
#-------------------------------------------------

QT       += core gui \
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArbitrageBot
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

#INCLUDEPATH += exchange \

win32:INCLUDEPATH = "C:/Program Files (x86)/OpenSSL-Win32/include/" \
                    exchange

SOURCES += \
        main.cpp \
        MainWindow.cpp \
        dock\dock_host.cpp \
        CAddPlatformDialog.cpp \
        exchange/CExchange.cpp \
    exchange/CDX.cpp \
    exchange/CBinance.cpp \
    exchange/CHITBTC.cpp \
    CLogger.cpp \
    exchange/CBittrex.cpp \
    exchange/CYobit.cpp

HEADERS += \
        MainWindow.h \
        dock\dock_host.h \
        CAddPlatformDialog.h \
        exchange/CExchange.h \
    exchange/CDX.h \
    exchange/CBinance.h \
    exchange/CHITBTC.h \
    CLogger.h \
    exchange/CBittrex.h \
    exchange/CYobit.h

FORMS += \
        MainWindow.ui \
    CAddPlatformDialog.ui

win32:LIBS += "C:/Program Files (x86)/OpenSSL-Win32/lib/libcrypto.lib"  \
                "C:/Program Files (x86)/OpenSSL-Win32/lib/libssl.lib"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
