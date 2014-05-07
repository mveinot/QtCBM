#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T20:26:24
#
#-------------------------------------------------

QT       += core gui

include(../qt-solutions/qtsingleapplication/src/qtsingleapplication.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtCBM
TEMPLATE = app

SOURCES += main.cpp\
        filewindow.cpp \
    settingsdialog.cpp \
    aboutdialog.cpp \
    detailsinfodialog.cpp

HEADERS  += filewindow.h \
    settingsdialog.h \
    aboutdialog.h \
    detailsinfodialog.h

FORMS    += filewindow.ui \
    settingsdialog.ui \
    aboutdialog.ui \
    detailsinfodialog.ui

RESOURCES = QtCBM.qrc
RC_ICONS = QtCBM.ico

OPENCBM_BIN_UTILS.files = bundle/bin
OPENCBM_BIN_UTILS.path = Contents/MacOS
OPENCBM_PLUGINS.files = bundle/plugins
OPENCBM_PLUGINS.path = Contents/MacOS

QMAKE_BUNDLE_DATA += OPENCBM_BIN_UTILS
QMAKE_BUNDLE_DATA += OPENCBM_PLUGINS
