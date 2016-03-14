QT       += core gui network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = developex-http-find
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        general.cpp \
        thread_pool.cpp \
        worker.cpp \


HEADERS  += mainwindow.h \
        general.h \
        thread_pool.h \
        worker.h



FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
