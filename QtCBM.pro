#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T20:26:24
#
#-------------------------------------------------

QT       += core gui

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
