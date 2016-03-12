QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = developex-http-find
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    httpfind.cpp

HEADERS  += mainwindow.h \
    httpfind.h

FORMS    += mainwindow.ui
