#-------------------------------------------------
#
# Project created by QtCreator 2013-03-13T18:29:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Extreme_Pong_Legend_232
TEMPLATE = app


SOURCES += main.cpp\
        pong.cpp \
    mainwindow.cpp \
    setportform.cpp \
    settingsform.cpp

HEADERS  += \
    mainwindow.hpp \
    pong.hpp \
    setportform.hpp \
    settingsform.hpp

FORMS    += \
    mainwindow.ui \
    setportform.ui \
    settingsform.ui

include( qextserialport/qextserialport.pri )
